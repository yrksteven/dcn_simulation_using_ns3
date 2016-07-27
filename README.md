# dcn_simulation_using_ns3
This is the project for the USTC course CS05112, which was finished last year.

Datacenter Network Simulation using ns3

Objective:
Simulate a datacenter network using ns3.
Experiment two traffic patterns.
Examine the network.
Improve the network.

Datacenter network topology
Description
8 servers: n1~8; 4 ToR switches: t1~4; 2 aggregation switches: a1~2; 1 core switch: c1
The network is partitioned into two clusters
The links connecting to c1 are PPP, or the other networks are Ethernets, the networks’ capacities are shown on the topology graph.
All the end-end delays on the networks are 500ns.
How the IP addresses are assigned can be found on the topology graph.
All the switches behaves like OSPF routers.

Traffic patterns
Pattern 1: inter-cluster traffic
Each server communicates using TCP with another server that comes from different cluster
For example, 1-5, 6-2, 3-7, 8-4
Pattern 2: many-to-one traffic
Select one server as the sink, and all the other servers communicate to it
Simulate the two patterns separately, obtain the throughput that the network can achieve, and find out the network bottleneck, how to improve the network.

