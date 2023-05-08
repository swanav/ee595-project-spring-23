# Machine Learning based TCP Congestion Control

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

## Directory structure
- dashboard
  - Contains the React Project implementing the Web Dashboard in the `dash` directory.
  - Contains the ExpressJs based Backend server for the web dashboard which orchestrates simulations and provides APIs for the frontend to view results of the simulation
- linux-module
  - Contains our attempt at creating a Linux Kernel Module for use in actual linux kernels. (This effort was abandoned due to Software Challenges as mentioned in the report.)
- ml-experiments
  - Contains some attempts at using classified learning to attempt loss differentiation.
- reinforcement learning
  - Contains our attempts to understand the concepts around reinforcement learning. Contains some sample programs from the pytorch library and attempts at creating a Gym environment for the TCP-ML library.
- simulator
  - Contains the NS-3 simulation
- submissions
  - Contain the previous submitted reports in md format.

## Getting Started

- First install the required dependencies.

- Download the ns3 Network simulator in the `simulator` directory by entering the following command
```bash
~/simulator $ ./simulator_init.sh
```

### Start the web application

- Start the backend server by navigating to `dashboard -> back` directory and entering the following command
```bash
~/dashboard/back $ npm run dev
```

- Start the web dashboard by navigating to `dashboard -> dash` directory and entering the following command
```bash
~/dashboard/dash $ npm start
```

### Simulate and train the model.

- Navigate to the directory with the NS-3 simulation and enter the following commands in two separate terminals
```bash
~/simulator/ns-allinone-3.38/ns-3.38 $ python scratch/congestion_simulations/rl_agent.py
```
```bash
~/simulator/ns-allinone-3.38/ns-3.38 $ export NS_GLOBAL_VALUE=SharedMemoryKey=4321;SharedMemoryPoolSize=4096;
~/simulator/ns-allinone-3.38/ns-3.38 $ ./ns3 run scratch/congestion_simulations/main.cc -- --transport_prot=TcpMl
```
