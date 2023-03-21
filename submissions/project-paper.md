# Machine Learning based TCP Congestion Control (ML-TCPCC)

> **Abstract**
> 
> Network congestion is a major problem that can lead to slow or unreliable network performance. Congestion control algorithms have been developed to mitigate this problem, but selecting the most appropriate algorithm for a given network environment can be challenging. This paper outlines an attempt to use machine learning to monitor network activity parameters and select the most appropriate congestion control parameters in real-time.

## Introduction

One of the most popular protocols used in computer networks is Transmission Control Protocol (TCP). Making sure that the network is utilized effectively without becoming overburdened is one of the main challenges faced by TCP. TCP Congestion Control algorithms control how much data can be transmitted over a network simultaneously, avoiding congestion and ensuring seamless transmission. Since the first Internet congestion collapse in Oct 1986, the Congestion Control issue has been a research focus for over three decades. Since then, various strategies have been put forth with varying degrees of success to enhance the functionality of TCP Congestion Control algorithms.

Machine learning (ML) has become a promising strategy in recent years for dealing with complex issues that are highly unpredictable or random. One such issue is TCP Congestion Control, where the underlying traffic patterns and network behavior can be highly unpredictable. Traditional TCP Congestion Control algorithms use a deterministic, rule-based approach that might only sometimes be the best choice. By gaining knowledge from the network's behavior and quickly adapting to changing circumstances, machine learning techniques can help make the current implementations better. In order to improve performance and reliability, this paper investigates a novel machine learning-based TCP congestion control method.

## TCP Congestion Control Algorithms

TCP Tahoe was the first TCP variant with in-built congestion control algorithms. It starts with a Slow Start phase exponentially increasing the cwnd till a certain SS threshold after which it increases linearly. On detecting a congestion event (3 duplicate acknowledgments or receiving a timeout), it backs off drastically, bringing down the cwnd to 1 MSS. This behavior is very extreme since there are many other factors in play.

TCP Reno and NewReno improve upon this approach by introducing fast retransmit and a fast recovery phase. They also handle congestion events differently depending upon their severity.

A congestion control algorithm called TCP Cubic was created to benefit from the high bandwidth levels present in modern communication links. In the face of high latency, it can establish high bandwidth connections over networks more quickly and reliably than earlier algorithms. Due to its greater scalability and flexibility, it has been adopted widely in high-speed long-delay networks and is currently the default algorithm implemented in the Linux kernel.

## TCP Congestion Control in the Linux Network Stack

The Linux networking stack includes a variety of congestion control algorithms, each with a different approach to congestion avoidance and rate control. These algorithms are implemented as separate modules in the kernel, which can be selected based on the network conditions and the application requirements. The TCP congestion control mechanism in the Linux networking stack is highly configurable, allowing system administrators to customize the congestion control algorithm and adjust the network parameters to optimize performance.

A user can check for available algorithms in the kernel by
```bash
$ cat/proc/sys/net/ipv4/tcp_available_congestion_control
> reno cubic
```

To check current congestion control algorithm being used by the Linux kernel for TCP connections:

```bash
$ cat/proc/sys/net/ipv4/tcp_congestion_control
> cubic
```

The Linux kernel allows dynamically switch between different congestion control algorithms at runtime without the need for a system reboot or any disruption to network services.

```bash
$ sudo sysctl net.ipv4.tcp_congestion_control=reno
```

We are creating a kernel module which will be added to the above list of available algorithms and can be selected in order to provide better transmission control with the help of the devised learning algorithm.

The learning module takes input parameters from the ongoing network activity, such as RTT, Packet Drops, Timeouts, and Duplicate ACKs, and also uses the current cwnd to calculate the next cwnd. It will try to predict a congestion event from the network parameters and slow down the cwnd increase to mitigate the congestion.

## Implementation

## Test Setup

A network simulator like NS3 allows us to simulate network activity for customized network topology. It saves us from having to use actual devices in order to simulate network activity. The Direct Code Execution (DCE) environment allows us to directly use our custom kernel module in the simulated nodes’ kernels.

We are developing a test utility to use the developed module on an elaborate setup with multiple devices competing for a single link resource, modeling congestion.

The setup to test the developed machine learning model is as follows.

The user opens the Python web application that will allow them to configure the network topology and the TCP congestion control module. This configuration will include the number of nodes, the bandwidth, and the delay.

The web application sends the selected network topology and TCP congestion control module configuration to the NS3 simulator.

NS3 installs the specified congestion control module on the nodes and generates TCP traffic. While the transmission is incomplete, the congestion control module keeps adapting the TCP transmission parameters for further transmissions until the data is completely transmitted. 

The performance of the TCP congestion control module is analyzed based on the network and the transmission parameters. NS3 sends the analysis back to the web application to display the analysis.

## Results and Observations
We plan to collect the data from these test runs in order to analyze them and infer some observations from them.

## Conclusion

## References

1. Linux Kernel Documentation (Building External Modules) - https://docs.kernel.org/kbuild/modules.html?highlight=modules
1. NS3’s Direct Code Execution Framework - https://ns-3-dce.readthedocs.io/en/latest/dce-user-kernel.html
1. J. Himeedah, A. Hamouda, M. Taher and S. Sati, "TCP Congestion Analysis of Wired and Wireless Links Using NS3," 2022 IEEE 2nd International Maghreb Meeting of the Conference on Sciences and Techniques of Automatic Control and Computer Engineering (MI-STA), Sabratha, Libya, 2022, pp. 361-365, doi: 10.1109/MI-STA54861.2022.9837740.
1. Ramneek, S. -J. Cha, S. H. Jeon, Y. J. Jeong, J. M. Kim and S. Jung, "Analysis of Linux Kernel Packet Processing on Manycore Systems," TENCON 2018 - 2018 IEEE Region 10 Conference, Jeju, Korea (South), 2018, pp. 2276-2280, doi: 10.1109/TENCON.2018.8650173.
1. Yiming Kong, Hui Zang, and Xiaoli Ma. 2018. Improving TCP Congestion Control with Machine Intelligence. In Proceedings of the 2018 Workshop on Network Meets AI & ML (NetAI'18). Association for Computing Machinery, New York, NY, USA, 60–66.
1. T. Doan and J. Kalita, "Selecting Machine Learning Algorithms Using Regression Models," 2015 IEEE International Conference on Data Mining Workshop (ICDMW), Atlantic City, NJ, USA, 2015, pp. 1498-1505, doi: 10.1109/ICDMW.2015.43.
1. A. Kuzmanovic and E. W. Knightly, "TCP-LP: a distributed algorithm for low priority data transfer," IEEE INFOCOM 2003. Twenty-second Annual Joint Conference of the IEEE Computer and Communications Societies (IEEE Cat. No.03CH37428), San Francisco, CA, USA, 2003, pp. 1691-1701 vol.3, doi: 10.1109/INFCOM.2003.1209192.
1. Sangtae Ha, Injong Rhee, and Lisong Xu. 2008. CUBIC: a new TCP-friendly high-speed TCP variant. SIGOPS Oper. Syst. Rev. 42, 5 (July 2008), 64–74.
1. R. I. Ledesma Goyzueta and Y. Chen, "A deterministic loss model based analysis of CUBIC," 2013 International Conference on Computing, Networking and Communications (ICNC), San Diego, CA, USA, 2013, pp. 944-949, doi: 10.1109/ICCNC.2013.6504217.
1. J. Wang, J. Wen, J. Zhang and Y. Han, "TCP-FIT: An improved TCP congestion control algorithm and its performance," 2011 Proceedings IEEE INFOCOM, Shanghai, China, 2011, pp. 2894-2902, doi: 10.1109/INFCOM.2011.5935128.
1. J. S. Ivey, B. P. Swenson and G. F. Riley, "Simulating networks with NS-3 and enhancing realism with DCE," 2017 Winter Simulation Conference (WSC), Las Vegas, NV, USA, 2017, pp. 690-704, doi: 10.1109/WSC.2017.8247825.
1. Pasi, Sarolahti., Alexey, Kuznetsov. "Congestion Control in Linux TCP."  null (2002).:49-62.
