# Optimal TCP Congestion Control Algorithm Selection with Machine Learning


- **Rutuparna K Patkar**
  - *rpatkar@usc.edu*
  - [@rutuparnakp](https://github.com/rutuparnakp)

- **Swanav Swaroop**
  - *swanavsw@usc.edu*
  - [@swanav](https://github.com/swanav)

## Problem Description

Network congestion is a major problem that can lead to slow or unreliable network performance. Congestion control algorithms have been developed to mitigate this problem, but selecting the most appropriate algorithm for a given network environment can be challenging. This proposal outlines a project to use machine learning to monitor network activity parameters and select the most appropriate congestion control algorithm in real-time.

## Work so far

We first learned about the Congestion Control in TCP and it’s significance as a major problem in Computer Science during our last semester’s course, EE450 : Introduction to Computer Networking. In addition to discussing the title and the scope with Prof. Nazarian, we also discussed the feasibility of the scope of this project with Prof Krishnamachari and both feel that the problem is approachable. With the advent of Machine Learning and Artificial Intelligence, it makes sense to try out a new approach to the age-old problem with hopes to achieve significant improvement.

To understand the feasibility of this attempt, we referred to the following papers.

- When machine learning meets congestion control: A survey and comparison, Huiling Jiang, Qing Li, Yong Jiang, GengBiao Shen, Richard Sinnott, Chen Tian, Mingwei Xu
- Congestion Control: A Renaissance with Machine Learning, Wenting Wei, Huaxi Gu, and Baochun Li
- A machine learning approach to improve congestion control over wireless computer networks, Geurts, Pascal & El-Khayat, Ibtissam & Leduc, Guy.

Reading the experiments conducted by these people leads us to believe that this area is being actively researched upon and something worthwhile and realistic.

## Algorithm Challenges

Selection of the correct Machine Learning algorithm for making appropriate inferences will be a challenge. There are a wide variety of models available, and choosing the one that best suits the task at hand will be crucial for the success of the project. Identifying the most relevant network activity parameters to use as inputs to the machine learning model will be crucial for the success of the project.

Choosing the most appropriate congestion control algorithm for a given network environment is also a complicated task. We will need to run experiments in order to determine the suitable algorithms for a certain network condition. We will draw comparisons between performance of various algorithms in a certain network condition and use the information to train the model.

For training, a lot of heavy-lifting can be done in Python with help of various libraries like Tensorflow, scipy. But once the model needs to be deployed, developing a C/C++ implementation will also be a challenge.

## Mathematical Challenges

Since, the TCP CC problem involves with a lot of random behavior, we will need to incorporate Probabilistic Models like Markov Chains. We will also need to do Time-Series Analysis and possibly forecasting in order to study and predict the network behavior. To analyze the incoming data from the network traffic, we might need to do statistical modeling like Linear Regression.

## Tool Challenges

The kernel module which swaps the congestion control algorithm will be implemented in C. It will be interacting with the ML model using C++ wrappers. The model can be developed using python with libraries like Tensorflow, scipy for deployment. The inputs to our program will be collected from the networking stack of Linux in form of various network parameters like RTT, Packet Drop events, ACK Timeouts etc. and used by the ML algorithm to select an appropriate Congestion Control algorithm and load it in the Linux kernel.

We will probably need to design some elementary tools to be able to measure the benefits provided by the selected algorithm and design a controlled environment using a Network Conditioner which simulates different network conditions.

The tool will show the Congestion Control algorithm in use and potentially show some graphs which show the throughput of data transfer in the controlled environment.

## Tentative Timeline

### Phase 1

- Analysis of different existing TCP congestion control mechanisms.
- Create a flow chat that covers many phases such as monitoring network activity, providing input to a machine learning model, and supplying its output to the linux kernel to adjust the congestion mechanism.
- Start documenting the work done. Writing the abstract and the current research done.

**Machine learning Perspective**
- Studying and choosing potentially appropriate models.
- Selection of input network parameters needed to train the machine learning model to select optimal CC method based on network topology, network conditions, and network traffic patterns, Latency, Throughput Requirements, Range of round-trip times the protocol may encounter.
- Creating a dataset by running experiments in a controlled environment.

**Linux Kernel Perspective**
- Analysis of how a congestion management strategy is chosen in the Linux kernel.
- Study into how to give the result of a machine learning model, i.e. a chosen tcp congestion control mechanism, to the linux kernel.

### Phase 2

- Train the Machine Learning Models using the selected Network Parameters.
- Develop a Linux Kernel Module to be loaded at runtime which switches between different TCP CC Algorithms.
- Run experiments in a controlled environment using Network Link Conditioner and collect data for further training of the model and make improvements in the selection strategy.
- Develop GUI tool to trigger different network conditions, run tests, and see the network performance results.
- Document the description of the selected Machine Learning model and a summary of the inferences from the experimental results.

### Phase 3

- Incorporate suggestions for improvements from the Instructors.
- Complete documentation by adding final results and references to the referenced research papers.
