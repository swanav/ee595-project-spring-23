import gymnasium as gym

import math
import os
import random
import matplotlib
import matplotlib.pyplot as plt
from itertools import count

import torch
import torch.optim as optim
from torch.nn import SmoothL1Loss

from replay_memory import ReplayMemory, Transition
from dqn import DQN

if not os.path.exists('model.pth'):
    env = gym.make("CartPole-v1")
else:
    env = gym.make("CartPole-v1", render_mode="human")

plt.ion()

cpu = torch.device("cpu")
# gpu = torch.device("mps") # if gpu is to be used

torch.set_default_device(cpu)

# Hyperparameters
num_episodes = 1000
BATCH_SIZE = 128
GAMMA = 0.99
EPS_START = 0.9
EPS_END = 0.05
EPS_DECAY = 1000
TAU = 0.005
LR = 1e-4

def select_action(state, steps_done, policy_net):
    sample = random.random()
    eps_threshold = EPS_END + (EPS_START - EPS_END) * \
        math.exp(-1. * steps_done / EPS_DECAY)
    if sample > eps_threshold:
        with torch.no_grad():
            return policy_net(state).max(1)[1].view(1, 1)
    else:
        return torch.tensor([[env.action_space.sample()]])

def plot_durations(episode_durations, show_result = False):
    plt.figure(1)
    durations_t = torch.tensor(episode_durations, dtype=torch.float).to(cpu)
    if show_result:
        plt.title('Result')
    else:
        plt.clf()
        plt.title('Training...')
    plt.xlabel('Episode')
    plt.ylabel('Duration')
    plt.plot(durations_t.numpy())
    if len(durations_t) >= 50:
        means = durations_t.unfold(0, 50, 1).mean(1).view(-1)
        means = torch.cat((torch.zeros(49).to(cpu), means))
        plt.plot(means.numpy())

    plt.pause(0.001)

def optimize_model(optimizer, memory, policy_net, target_net):
    if len(memory) < BATCH_SIZE:
        return
    
    transitions = memory.sample(BATCH_SIZE)
    batch = Transition(*zip(*transitions))
    
    non_final_mask = torch.tensor(tuple(map(lambda s: s is not None,
                                          batch.next_state)), dtype=torch.bool)
    non_final_next_states = torch.cat([s for s in batch.next_state if s is not None])

    state_batch = torch.cat(batch.state)
    action_batch = torch.cat(batch.action)
    reward_batch = torch.cat(batch.reward)

    # Compute Q(s_t, a) - the model computes Q(s_t), then we select the
    # columns of actions taken. These are the actions which would've been taken
    # for each batch state according to policy_net
    state_action_values = policy_net(state_batch).gather(1, action_batch.long())

    # Compute V(s_{t+1}) for all next states.
    # Expected values of actions for non_final_next_states are computed based
    # on the "older" target_net; selecting their best reward with max(1)[0].
    # This is merged based on the mask, such that we'll have either the expected
    # state value or 0 in case the state was final.
    next_state_values = torch.zeros(BATCH_SIZE)
    with torch.no_grad():
        next_state_values[non_final_mask] = target_net(non_final_next_states).max(1)[0]
    # Compute the expected Q values
    expected_state_action_values = (next_state_values * GAMMA) + reward_batch

    # Compute Huber loss
    criterion = SmoothL1Loss()
    loss = criterion(state_action_values, expected_state_action_values.unsqueeze(1))

    # Optimize the model
    optimizer.zero_grad()
    loss.backward()
    # In-place gradient clipping
    torch.nn.utils.clip_grad_value_(policy_net.parameters(), 100)
    optimizer.step()

def main():

    n_actions = env.action_space.n

    state, info = env.reset(seed=42)
    n_observations = len(state)

    #  Train the model only if it is not already trained
    if not os.path.exists('model.pth'):
        policy_net = DQN(n_observations, n_actions)
        target_net = DQN(n_observations, n_actions)
        target_net.load_state_dict(policy_net.state_dict())

        optimizer = optim.AdamW(policy_net.parameters(), lr=LR, amsgrad=True)
        memory = ReplayMemory(10000)

        steps_done = 0

        episode_durations = []


        for _ in range(num_episodes):
            # Initialize the environment and get it's state
            state, info = env.reset()
            state = torch.tensor(state, dtype=torch.float32).unsqueeze(0)
            for t in count():
                action = select_action(state, steps_done, policy_net)
                steps_done += 1
                observation, reward, terminated, truncated, _ = env.step(int(action.item()))
                # env.render()
                reward = torch.tensor([reward])
                done = terminated or truncated

                if terminated:
                    next_state = None
                else:
                    next_state = torch.tensor(observation, dtype=torch.float32).unsqueeze(0)

                # Store the transition in memory
                memory.push(state, action, next_state, reward)

                # Move to the next state
                state = next_state

                # Perform one step of the optimization (on the policy network)
                optimize_model(optimizer, memory, policy_net, target_net)

                # Soft update of the target network's weights
                # θ′ ← τ θ + (1 −τ )θ′
                target_net_state_dict = target_net.state_dict()
                policy_net_state_dict = policy_net.state_dict()
                for key in policy_net_state_dict:
                    target_net_state_dict[key] = policy_net_state_dict[key]*TAU + target_net_state_dict[key]*(1-TAU)
                target_net.load_state_dict(target_net_state_dict)

                if done:
                    episode_durations.append(t + 1)
                    plot_durations(episode_durations)
                    break

        # Save the model
        torch.save(policy_net.state_dict(), 'model.pth')

        print('Complete')
        plot_durations(episode_durations, True)
        plt.ioff()
        plt.show()
    else:
        policy_net = DQN(n_observations, n_actions)
        policy_net.load_state_dict(torch.load('model.pth'))

        # Test the model
        state, info = env.reset()
        state = torch.tensor(state, dtype=torch.float32).unsqueeze(0)
        for t in count():
            action = policy_net(state).max(1)[1].view(1, 1)
            observation, reward, terminated, truncated, _ = env.step(int(action.item()))
            env.render()
            done = terminated or truncated
            if terminated:
                next_state = None
            else:
                next_state = torch.tensor(observation, dtype=torch.float32).unsqueeze(0)

            state = next_state

            if done:
                print('Episode finished after {} timesteps'.format(t+1))
                break
    
        env.close()

        print('Complete. Model iter ')


if __name__ == '__main__':
    main()