from py_interface import *
from ctypes import *
import os
import torch
import numpy as np
import math
import torch.nn as nn
import torch.nn.functional as F
from collections import deque
import matplotlib.pyplot as plt
import pymongo
import random
import sys
import hashlib

# exp = Experiment(1234, 4096, 'rl-tcp', '../../', using_waf=False)
# exp.run(show_output=1)
# 'NS_GLOBAL_VALUE': 'SharedMemoryKey=1234;SharedMemoryPoolSize=4096;
# export NS_GLOBAL_VALUE="SharedMemoryKey=1234;SharedMemoryPoolSize=4096;"

class TcpRlEnv(Structure):
    _pack_ = 1
    _fields_ = [
        ('nodeId', c_uint32),
        ('socketUid', c_uint32),
        ('envType', c_uint8),
        ('simTime_us', c_int64),
        ('ssThresh', c_uint32),
        ('cwnd', c_uint32),
        ('segmentSize', c_uint32),
        ('segmentsAcked', c_uint32),
        ('bytesInFlight', c_uint32),
        # ('rtt', c_int64),
    ]

class TcpRlAct(Structure):
    _pack_ = 1
    _fields_ = [
        ('new_ssThresh', c_uint32),
        ('new_cWnd', c_uint32)
    ]

class RlAgentNeuralNetwork(nn.Module):
    def __init__(self, n_observations = 5, n_actions = 4, n_hidden = 20):
        super(RlAgentNeuralNetwork, self).__init__()
        self.input_layer = nn.Linear(n_observations, n_hidden)
        self.hidden_layer = nn.Linear(n_hidden, n_hidden)
        self.output_layer = nn.Linear(n_hidden, n_actions)

    def forward(self, x):
        x = F.relu(self.input_layer(x))
        x = F.relu(self.hidden_layer(x))
        x = self.output_layer(x)
        return x

class State():
    def __init__(self, ssThresh: int, cwnd: int, segmentSize: int, segmentsAcked: int, bytesInFlight: int):
        self.ssThresh: int = ssThresh
        self.cwnd: int = cwnd
        self.segmentSize: int = segmentSize
        self.segmentsAcked: int = segmentsAcked
        self.bytesInFlight: int = bytesInFlight

    def toTensor(self):
        return torch.tensor(self.toArray(), dtype=torch.float32)

    def toArray(self):
        return [self.ssThresh, self.cwnd, self.segmentSize, self.segmentsAcked, self.bytesInFlight]

    def __dict__(self) -> dict[str, int]:
        return {
            'ssThresh': self.ssThresh,
            'cwnd': self.cwnd,
            'segmentSize': self.segmentSize,
            'segmentsAcked': self.segmentsAcked,
            'bytesInFlight': self.bytesInFlight
        }

def CreateState(state: dict) -> State:
    # print('CreateState', state)
    return State(state['ssThresh'], state['cwnd'], state['segmentSize'], state['segmentsAcked'], state['bytesInFlight'])

def StateFromEnv(env):
    ssThresh = env.ssThresh
    cwnd = env.cwnd
    segmentsAcked = env.segmentsAcked
    segmentSize = env.segmentSize
    bytesInFlight = env.bytesInFlight
    return State(ssThresh, cwnd, segmentSize, segmentsAcked, bytesInFlight)

class Transition():
    def __init__(self, state: State, action: int, next_state: State, reward: int):
        self.state: State = state
        self.action: int = action
        self.next_state: State = next_state
        self.reward: int = reward

    def id(self):
        return hashlib.md5(str(self.__dict__()).encode()).hexdigest()

    def __dict__(self, with_id = False) -> dict[str, any]:
        r = {
            'state': self.state.__dict__(),
            'action': self.action,
            'next_state': self.next_state.__dict__(),
            'reward': self.reward
        }
        if with_id:
            r['_id'] = self.id()
        return r

def CreateTransition(transition: dict) -> Transition:
    # print('CreateTransition', transition)
    return Transition(CreateState(transition['state']), transition['action'], CreateState(transition['next_state']), transition['reward'])

class ReplayMemory():
    def __init__(self, max_capacity = 1000):
        self.memory = deque(maxlen=max_capacity)

    def push(self, transition: Transition):
        self.memory.append(transition)

    def sample(self, batch_size) -> list[Transition]:
        return random.sample(self.memory, batch_size)

    def __len__(self):
        return len(self.memory)

class RlAgent(object):
    def __init__(self):
        self.batchsize = 32
        self.gamma = 0.9
        self.epsilon_start = 0.9
        self.epsilon_end = 0.05
        self.epsilon_decay = 1000
        self.tau = 0.005
        self.learning_rate = 0.0001

        self.policy_net = RlAgentNeuralNetwork(5, 4, 50)
        self.target_net = RlAgentNeuralNetwork(5, 4, 50)
        self.target_net.load_state_dict(self.policy_net.state_dict())

        self.optimizer = torch.optim.AdamW(self.policy_net.parameters(), lr=self.learning_rate, amsgrad=True)

        self.learn_step = 0
        self.target_replace = 100
        self.loss_func = nn.MSELoss()
        self.memory = ReplayMemory()
        self.memory_counter = 0
        self.age_memory = 1000

    def save(self, path: str):
        torch.save(self.policy_net.state_dict(), path)

    def load(self, path: str):
        self.policy_net.load_state_dict(torch.load(path))
        self.target_net.load_state_dict(torch.load(path))

    def choose_action(self, x: State) -> int:
        sample = random.random()
        eps_threshold = self.epsilon_end + (self.epsilon_start - self.epsilon_end) * math.exp(-1. * self.memory_counter / self.epsilon_decay)
        self.learn_step += 1
        if sample > eps_threshold:
            # exploit
            with torch.no_grad():
                return self.policy_net(x.toTensor()).max(0)[1].view(1, 1)
        else:
            # explore
            return torch.tensor([[random.randint(0, 3)]])

    def store_transition(self, transition: Transition):
        self.memory.push(transition)
        self.memory_counter += 1

    def learn(self):
        if self.memory_counter < self.age_memory:
            return
        try:
            self.learn_step += 1
            if self.learn_step % self.target_replace == 0:
                self.target_net.load_state_dict(self.policy_net.state_dict())

            # choose a mini batch
            batch = self.memory.sample(self.batchsize)
            states: torch.Tensor = torch.Tensor([s.state.toArray() for s in batch])
            actions: torch.LongTensor = torch.LongTensor([[s.action] for s in batch])
            rewards: torch.LongTensor = torch.LongTensor([[s.reward] for s in batch])
            # Compute only the non-final states
            # next_states: torch.Tensor = torch.Tensor([s.next_state.toArray() for s in batch if s.next_state is not None])
            next_states: torch.Tensor = torch.Tensor([s.next_state.toArray() for s in batch])


            state_action_values = self.policy_net(states).gather(1, actions)
            next_state_values = torch.zeros(self.batchsize)
            with torch.no_grad():
                next_state_values = self.target_net(next_states).max(1)[0].detach()
            expected_state_action_values = (next_state_values * self.gamma) + rewards

            criterion = nn.SmoothL1Loss()
            loss = criterion(state_action_values, expected_state_action_values.unsqueeze(1))

            # q_next = self.target_net(next_states).detach()
            # q_target = rewards + 0.8 * q_next.max(1, True)[0].data

            # loss = self.loss_func(q_eval, q_target)

            self.optimizer.zero_grad()
            loss.backward()
            self.optimizer.step()
        except:
            print('batch', batch)
            print('states', states)
            print('actions', actions)
            print('rewards', rewards)
            print('next_states', next_states)

def getEnvActions(action: int, state: State) -> tuple[int, int]:
    new_cwnd = state.cwnd
    new_ssThresh = state.ssThresh
    if action == 0:
        new_cwnd += state.segmentSize * 2
    elif action == 1:
        new_cwnd += state.segmentSize
    elif action == 2:
        new_cwnd -= state.segmentSize * 2
    elif action == 3:
        new_cwnd -= state.segmentSize
    new_ssThresh = max(2, int(state.cwnd / 2))
    return new_cwnd, new_ssThresh

def get_reward(state: State, next_state: State) -> int:
    alpha, beta = 1, 0.5
    bytes_acked = next_state.segmentsAcked * next_state.segmentSize
    throughput = alpha * next_state.cwnd
    reliability = beta * pow(bytes_acked - next_state.bytesInFlight - state.cwnd, 2, 1)
    reward = math.floor(throughput - reliability)
    return reward

def main():
    Init(4321, 4096)
    var = Ns3AIRL(4321, TcpRlEnv, TcpRlAct)
    Reset()

    agent = RlAgent()

    # If the model exists, load it
    # if os.path.exists('model.pt'):
    #     agent.load('model.pt')

    try:
        print('Start')
        while not var.isFinish():
            with var as data:
                if not data:
                    print('break')
                    break
                state: State = StateFromEnv(data.env)
                action: int = agent.choose_action(state)

                new_cWnd, new_ssThresh = getEnvActions(action, state)

                data.act.new_cWnd = new_cWnd
                data.act.new_ssThresh = new_ssThresh

                next_state: State = StateFromEnv(data.env)

                reward: int = get_reward(state, next_state)
                print(reward)

                transition = Transition(state, action, next_state, reward)
                agent.store_transition(transition)
                agent.learn()

    except KeyboardInterrupt:
        print('stop')
        agent.save('model.pt')

if __name__ == '__main__':
    main()
