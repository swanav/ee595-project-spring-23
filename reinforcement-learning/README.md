We are modelling the TCP Congestion Control Algorithm using Deep Reinforcement Learning, using OpenAI Gym to create a Environment which provides feedback to the agent.
- The environment is the network.
- The agent is our algorithm.
- The observations are the state of the network, the current congestion window, the slow start threshold, the current rtt, the throughput of the algorithm.
- The actions are the operations on the congestion window size, increasing, decreasing or no change.
- The reward are the number of acks. 
We have used the DQN algorithm to train the agent. 
The agent is trained and tested in a NS-3 simulated environment.
