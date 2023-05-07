import gymnasium as gym
import numpy as np
import socket
import os

from gymnasium.envs import CongestionControlEnv
from gymnasium.wrappers import ActionWrapper, ObservationWrapper
from gymnasium.wrappers import TimeLimit
from gymnasium.wrappers import Monitor
from gymnasium.wrappers import FlattenObservation
from gymnasium.wrappers import FrameStack
from gymnasium.wrappers import TransformReward
from gymnasium.wrappers import TransformObservation
from gymnasium.wrappers import TransformAction
from gymnasium.wrappers import TransformInfo
from gymnasium.wrappers import TransformDone


class RLAgent:
    def __init__(self, env):
        self.env = env

    def run(self):
        # Run the agent
        pass


class RLAgentWrapper:
    def __init__(self, env):
        self.env = env

    def run(self):
        # Run the agent
        pass




if __name__ == '__main__':
    # Create a Unix Domain Socket

    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as sock:

        # Unlink the socket if it already exists
        if os.path.exists('/tmp/rl_agent.sock'):
            os.unlink('/tmp/rl_agent.sock')

        # Bind the socket to the port
        sock.bind('/tmp/rl_agent.sock')
        sock.listen(1)

        try:
            # Accept connections forever
            while True:
                connection, client_address = sock.accept()
                try:
                    # Receive data from the socket
                    data = connection.recv(1024)
                    print(data)

                    # Send data back to the client
                    connection.sendall(np.array([1, 2, 3]))
                except KeyboardInterrupt as e:
                    print(e)
                finally:
                    # Clean up the connection
                    connection.close()
        except KeyboardInterrupt as e:
            print(e)

