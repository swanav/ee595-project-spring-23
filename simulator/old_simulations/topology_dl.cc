#include <iostream>
#include <fstream>
#include <string>

#include "ns3/command-line.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"
#include "ns3/error-model.h"
#include "ns3/tcp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"

// #include "ns3/ns3-ai-module.h"
// #include "tcp-rl.h"

struct ProgramArgs_t {
    bool verbose;
    std::string protocol;
    uint32_t node_count;
    uint64_t link_rate;
    uint64_t delay;
    uint32_t queue_size;
    uint32_t simulation_time;
    uint64_t payload_size;
} Args;

void SetDefaultProgramArgs(ProgramArgs_t& args) {
    args.verbose = false;
    args.protocol = "TcpNewReno";
    args.node_count = 2;
    args.link_rate = 1000000000;
    args.delay = 100;
    args.queue_size = 1000;
    args.simulation_time = 10;
    args.payload_size = 1000;
}

void ParseProgramArgs(int argc, char** argv, ProgramArgs_t& args) {
    ns3::CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "turn on log components", Args.verbose);
    cmd.AddValue("protocol", "congestion control protocol: TcpNewReno, TcpCubic, TcpMl", Args.protocol);
    cmd.AddValue("node-count", "number of nodes", Args.node_count);
    cmd.AddValue("link-rate", "link rate in bps", Args.link_rate);
    cmd.AddValue("delay", "link delay in ms", Args.delay);
    cmd.AddValue("queue-size", "queue size in packets", Args.queue_size);
    cmd.AddValue("payload-size", "payload size in bytes", Args.payload_size);
    cmd.AddValue("simulation-time", "simulation time in seconds", Args.simulation_time);
    cmd.Parse(argc, argv);
}

int main(int argc, char** argv) {

    NS_LOG_UNCOND("EE595 TCP with Reinforcement Learning");

    SetDefaultProgramArgs(Args);
    ParseProgramArgs(argc, argv, Args);

    // Create Experiment Topology
    

    return 0;
}

