#include "topology_1.hh"

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

void DumbellTopologyExperiment::Setup() {
    // NS_LOG_UNCOND("EE595 TCP with Reinforcement Learning");
    if (this->PreSetup) {
        this->PreSetup(this);
    }

    if (this->PostSetup) {
        this->PostSetup(this);
    }
}

void DumbellTopologyExperiment::Simulate() {
    ns3::Simulator::Stop(ns3::Seconds(this->simulation_time));

    ns3::Simulator::Run();
}
