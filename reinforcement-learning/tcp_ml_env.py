import gymnasium as gym
from gymnasium.envs.registration import register
import numpy as np

class TcpMlEnv(gym.Env):

    def __init__(self) -> None:
        super().__init__()

        # Define the state space
        # The state space should represent the current state of the TCP connection, 
        # which includes information about the congestion window, 
        # the number of unacknowledged packets, 
        # the round-trip time, etc.

        low_state = np.array([0])
        high_state = np.array([0])

        self.observation_space = gym.spaces.Box(
            low=low_state,
            high=high_state,
            dtype=np.float32,
        )

        # The action space should represent the available actions 
        # that the agent can take, which includes increasing or 
        # decreasing the congestion window size, increasing or 
        # decreasing the sending rate, etc.
        self.action_space = gym.spaces.Discrete(2)

    def step(self, action):
        print(action)
        # observation, reward, terminated, truncated, info
        return self.observation_space.sample(), 0, False, False, {}

    def reset(self, seed=None):
        super().reset(seed=seed)
        print("Resetting the environment")
        print(f"Seed: {seed}")
        print(f"Observation Space: {self.observation_space}")
        return self.observation_space.sample(), {}

        

    def render(self, mode='human'):
        pass

    def close(self):
        pass



    

register(
    id="TCP-ML-v1",
    entry_point="tcp_ml_env:TcpMlEnv",
    nondeterministic=True,
)
