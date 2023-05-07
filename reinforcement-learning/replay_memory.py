from collections import deque, namedtuple
import random

Transition = namedtuple('Transition', ('state', 'action', 'next_state', 'reward'))

class ReplayMemory(object):
    def __init__(self, capacity):
        """Initializes a replay memory with a maximum capacity."""
        self.memory = deque([], maxlen=capacity)
    def push(self, *args):
        """Saves a transition."""
        self.memory.append(Transition(*args))
    def sample(self, batch_size):
        """Randomly samples a batch of transitions."""
        return random.sample(self.memory, batch_size)
    def __len__(self):
        """Returns the current size of the replay memory."""
        return len(self.memory)
