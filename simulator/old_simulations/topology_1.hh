#include <iostream>

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

class DumbellTopologyExperiment {

    uint32_t node_count;
    uint64_t link_rate;
    uint64_t delay;
    uint32_t queue_size;
    uint32_t simulation_time;    

public:
    DumbellTopologyExperiment() : 
        node_count(2), 
        link_rate(1000000000), 
        delay(100), 
        queue_size(1000), 
        simulation_time(10)
     {};
    ~DumbellTopologyExperiment();

    void (*PreSetup)(DumbellTopologyExperiment* self);
    void (*PostSetup)(DumbellTopologyExperiment* self);
    void (*PreTearDown)(DumbellTopologyExperiment* self);
    void (*PostTearDown)();

private:
    void Setup();
    void Simulate();
    void Teardown();
};
