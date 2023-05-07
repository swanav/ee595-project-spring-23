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

        # The observation space should represent the current state of the TCP connection,
        # RTT can be represented as a float value with a range of 0 to infty.
        rtt_low = 0
        rtt_high = np.inf
        # The receiver window size can be represented as an integer value with a range of 0 to infty.
        receiver_window_size_low = 0
        receiver_window_size_high = np.inf
        # The congestion window size can be represented as a integer value with a range of 0 to receiver window size.
        congestion_window_size_low = 0
        congestion_window_size_high = np.inf
        # The number of unacknowledged packets can be represented as an integer value with a range of 0 to infty.
        unacknowledged_packets_low = 0
        unacknowledged_packets_high = np.inf
        # The number of duplicate acknowledgements can be represented as an integer value with a range of 0 to 10.
        duplicate_acknowledgements_low = 0
        duplicate_acknowledgements_high = 10
        # The number of retransmissions can be represented as an integer value with a range of 0 to infty.
        retransmissions_low = 0
        retransmissions_high = np.inf


        low_state  = np.array([rtt_low,  receiver_window_size_low,  congestion_window_size_low,  unacknowledged_packets_low,  duplicate_acknowledgements_low,  retransmissions_low])
        high_state = np.array([rtt_high, receiver_window_size_high, congestion_window_size_high, unacknowledged_packets_high, duplicate_acknowledgements_high, retransmissions_high])
        self.observation_space = gym.spaces.Box(low=low_state, high=high_state, dtype=np.float32)

        # The action space should represent the available actions 
        # that the agent can take, which includes increasing or 
        # decreasing the congestion window size or keeping it the same.
        self.action_space = gym.spaces.Discrete(2)

    def step(self, action):
        # print(action)
        # observation, reward, terminated, truncated, info
        return self.observation_space.sample(), 0, False, False, {}

    def reset(self, seed=None):
        super().reset(seed=seed)
        # print("Resetting the environment")
        # print(f"Seed: {seed}")
        # print(f"Observation Space: {self.observation_space}")
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
