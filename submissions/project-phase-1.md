# Machine Learning based TCP Congestion Control (ML-TCPCC)

## Problem Description

One of the most popular protocols used in computer networks is Transmission Control Protocol (TCP). Making sure that the network is utilized effectively without becoming overburdened is one of the main challenges faced by TCP. TCP Congestion Control algorithms control how much data can be transmitted over a network simultaneously, avoiding congestion and ensuring seamless transmission. Since the first Internet congestion collapse in Oct 1986, the Congestion Control issue has been a research focus for over three decades. Since then, various strategies have been put forth with varying degrees of success to enhance the functionality of TCP Congestion Control algorithms.

Machine learning (ML) has become a promising strategy in recent years for dealing with complex issues that are highly unpredictable or random. One such issue is TCP Congestion Control, where the underlying traffic patterns and network behavior can be highly unpredictable. Traditional TCP Congestion Control algorithms use a deterministic, rule-based approach that might only sometimes be the best choice. By gaining knowledge from the network's behavior and quickly adapting to changing circumstances, machine learning techniques can help make the current implementations better. In order to improve performance and reliability, this paper investigates a novel machine learning-based TCP congestion control method.

## Project Timeline

### Work so far
- Analysis of different existing TCP congestion control mechanisms.
- Create a flow chat that covers many phases such as monitoring network activity, providing input to a machine learning model, and supplying its output to the linux kernel to adjust the congestion mechanism.
- Start documenting the work done. Writing the abstract and the current research done.

### Machine learning Perspective
- Studying potentially appropriate models.
- Creating a dataset by running experiments in a controlled environment.

### Linux Kernel Perspective
- Analysis of how a congestion management strategy is chosen in the Linux kernel.
- Study into how to give the result of a machine learning model, i.e. a chosen tcp congestion control mechanism, to the linux kernel. - In Progress
- Develop GUI tool to trigger different network conditions, run tests, and see the network performance results. - In Progress

> ## Change in Plans
> We had earlier set out to select an existing TCP Congestion Control algorithm and swap out the kernel module to enhance the throughput. Our study since has indicated that this approach might limit the potential improvement that Machine Learning can provide. Hence, we have now shifted the scope to developing a TCP Congestion Control mechanism which uses Machine Learning.

### Phase 2
- Train the Machine Learning Model using the various Network Parameters.
- Develop a Linux Kernel Module to be loaded at runtime which switches between different TCP CC Algorithms.
- Run experiments in a controlled environment using NS-3 Network Simulator and collect data for further training of the model and make improvements in the selection strategy.
- Document the description of the selected Machine Learning model and a summary of the inferences from the experimental results.

### Phase 3
- Incorporate suggestions for improvements from the Instructors.
- Complete documentation by adding final results and references to the referenced research papers.

## Analysis

The setup to test the developed machine learning model is as follows.

The user opens the Python web application that will allow them to configure the network topology and the TCP congestion control module. This configuration will include the number of nodes, the bandwidth, and the delay.

The web application sends the selected network topology and TCP congestion control module configuration to the NS3 simulator.

NS3 installs the specified congestion control module on the nodes and generates TCP traffic. While the transmission is incomplete, the congestion control module keeps adapting the TCP transmission parameters for further transmissions until the data is completely transmitted. 

The performance of the TCP congestion control module is analyzed based on the network and the transmission parameters. NS3 sends the analysis back to the web application to display the analysis.

## Implementation

We are implementing a Linux Kernel Module, which will be loaded in the Linux Kernel to provide the implementation of our TCP Congestion Control algorithm.
NS-3, Network Simulation Library is used for the test setup. This will involve
- setting up a network topology with simulated nodes.
- installing the linux kernel on the simulated devices with support for our custom module using DCE.
- Installing relevant probes for measuring relevant network parameters.
- Generate traffic on the simulation and collect the performance data.

A web dashboard will present the demo tool and allow the user to select the network topology, number of simulated devices, test parameters and provide visualizations of the test results.

## Mathematical Challenges

Our current research points us to the use of Regression based Machine Learning models to determine the optimal cwnd and predict potential congestion events based on the evolution of the window RTT. We will also need to do Time-Series Analysis and possibly study forecasting methods in order to analyze and predict the network behavior.

## Software Challenges
The project works around the Linux Kernel. Additionally, in order to evaluate algorithm performance, a simulation testbed is being implemented in NS-3. Wireshark and tcpdump are used to analyze the generated packet traces based on the data traffic. And a GUI (as explained later). The languages being used will primarily be C, C++ and Python.

**What are the inputs and output of your software. What steps will you take to design and test the software?**

A user can add a custom kernel module to the Linux Kernel with the insmod command.

```bash
$ sudo insmod mltcpcc.ko
```

The Linux kernel allows dynamically switch between different congestion control algorithms at runtime without the need for a system reboot or any disruption to network services.

```bash
$ sudo sysctl net.ipv4.tcp_congestion_control=mltcpcc
```

Once loaded, the Congestion Control algorithm, will intercept the network activity and use the network parameters to control the rate of segment transmission.


**How much as interface work do you plan to add: Do you plan to deliver a tool with a GUI (graphical user interface)? If so, what tools/package will you use to avoid significant coding related to graphical part of your tool?**

We will be making a Web Dashboard to collect user input and display test output. 

A Python Backend, using Flask to develop APIs which will relay the commands and the test results to the user interface by calling the NS3 test program with the relevant invocation parameters.

Writing the abstract (section 1) of the paper/report. Writing most of Section 2 (introduction, i.e., prior work summary, the key novelties of your work) (30 points)


