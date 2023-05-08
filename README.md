# Machine Learning based TCP Congestion Control (ML-TCPCC)

- **Rutuparna K Patkar**
  - *rpatkar@usc.edu*
  - [@rutuparnakp](https://github.com/rutuparnakp)

- **Swanav Swaroop**
  - *swanavsw@usc.edu*
  - [@swanav](https://github.com/swanav)

This repository contains our work on TCP-ML, a deep reinforcement learning (DRL) algorithm for congestion control in network communication. Traditional congestion control algorithms use fixed strategies that may not be optimal for all network conditions and can result in suboptimal performance. TCP-ML leverages the power of DRL to learn congestion control strategies that optimize network performance in response to changing network conditions. The agent in TCP- ML selects actions that modify the congestion window and slow start threshold based on the feedback it receives from the environment, which includes throughput and reliability measures. We evaluate the performance of TCP-ML against traditional TCP algorithms using simulation experiments, and show that TCP-ML achieves higher throughput and reliability in a variety of network conditions. Our results demonstrate the potential of DRL algorithms to improve congestion control in network communication, and suggest that TCP-ML could be a promising approach for future research in this area.


## Problem Description

One of the most popular protocols used in computer networks is Transmission Control Protocol (TCP). Making sure that the network is utilized effectively without becoming overburdened is one of the main challenges faced by TCP. TCP Congestion Control algorithms control how much data can be transmitted over a network simultaneously, avoiding congestion and ensuring seamless transmission. Since the first Internet congestion collapse in Oct 1986, the Congestion Control issue has been a research focus for over three decades. Since then, various strategies have been put forth with varying degrees of success to enhance the functionality of TCP Congestion Control algorithms.

## Proposed Solution
We propose to use a Deep Reinforcement Learning based Dynamic TCP Congestion Control Algorithm. The algorithm will be trained using a simulator and the trained model will be used to control the congestion control algorithm in the Linux Kernel. The following figure shows the end-to-end solution.

## Dependencies

- NS-3 Network Simulator
  - NS3-AI Plugin 
- Node.JS
  - React
- MongoDb
- Python
  - PyTorch
