#include "ns3/applications-module.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/gnuplot.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet-socket-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-phy.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Network_Topology");

static int linkDataRate = 1024;
static int nodes = 20;
static int dataRate = 1024;
static std::string transport_prot = "TcpWestwoodPlus";

class Experiment
{
  public:
    class Result
    {
      public:
        Result()
        {
        }

        Result(long nodes,
               double rate,
               double rxThroughput,
               double avgTxThroughput,
               double avgBackoffTimePerTranmission)
            : nodes(nodes),
              rate(rate),
              rxThroughput(rxThroughput),
              avgTxThroughput(avgTxThroughput),
              avgBackoffTimePerTranmission(avgBackoffTimePerTranmission)
        {
        }

        long nodes;
        double rate;
        double rxThroughput;
        double avgTxThroughput;
        double avgBackoffTimePerTranmission;
    };

    Experiment(long nodes = 20, double rate = 1.)
        : nodes(nodes),
          rate(rate)
    {
        collisionRate = 0;
        collisionCount = 0;
        txCount = 0;
        rxThroughput = 0;
        avgTxThroughput = 0;
    }

    void Setup();

    void PostSetup();

    void Run();

    void PreTeardown();

    void Teardown();

    Result getResults();

  private:
    long nodes;
    double rate;
    double rxThroughput, avgTxThroughput;
    double collisionRate;
    uint32_t collisionCount, txCount;
    double avgBackoffTime;
    long backoffCount;

    NodeContainer txNodes, rxNodes, txApNode, rxApNode;
    NetDeviceContainer txDevices, rxDevices, txApDevice, rxApDevice, p2pDevices;
    YansWifiPhyHelper txWifiPhy, rxWifiPhy;
    PointToPointHelper p2pPhy;
    Ipv4InterfaceContainer txApInterface, rxApInterface, txInterfaces, rxInterfaces, p2pInterfaces;
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor;

    void CreateNodes();
    void SetupWiFi();
    void SetupIPStack();
    void SetupTCP();
    void SetupApplication();
    void PlaceNodes(int distance = 10.0);

    void OnBackoff(std::string context, uint32_t newVal, uint8_t /* linkId */);
    void OnCollision(Ptr<const Packet> packet);
    void OnTx(Ptr<const Packet> packet);

    uint32_t ContextToNodeId(std::string context);
};

uint32_t
Experiment::ContextToNodeId(std::string context)
{
    std::string sub = context.substr(10);
    uint32_t pos = sub.find("/Device");
    return stoi(sub.substr(0, pos));
}

// Trace function for backoff
void
Experiment::OnBackoff(std::string context, uint32_t newVal, uint8_t /* linkId */)
{
    uint32_t nodeId = this->ContextToNodeId(context);
    if (nodeId == 0)
    {
        this->avgBackoffTime =
            (this->avgBackoffTime * this->backoffCount + newVal) / (this->backoffCount + 1);
        this->backoffCount++;
    }
    // std::cout << "Backoff time=" << Simulator::Now() << " node=" << ContextToNodeId(context) << "val=" << newVal << std::endl;
}

void
Experiment::OnCollision(Ptr<const Packet> packet)
{
    std::cout << "Collision" << std::endl;
    this->collisionCount++;
}

void Experiment::OnTx(Ptr<const Packet> packet)
{
    // cout << "Sent" << endl;
    this->txCount++;
}

void
Experiment::CreateNodes()
{
    NS_LOG_UNCOND("\n");

    // Create TX Nodes
    this->txNodes.Create(this->nodes / 2);
    NS_LOG_UNCOND("Transmitter Nodes created.");

    // Create RX Nodes
    this->rxNodes.Create(this->nodes / 2);
    NS_LOG_UNCOND("Receiver Nodes created.");

    // Create AP Node
    this->txApNode.Create(1);
    this->rxApNode.Create(1);
    NS_LOG_UNCOND("AP Nodes created.");
}

void
Experiment::SetupWiFi()
{
    NS_LOG_UNCOND("\n");

    // Setup the PHY
    WifiHelper wifi;
    // Use the default 802.11g standard
    wifi.SetStandard(WIFI_STANDARD_80211g);

    // Setup the Wifi MAC
    WifiMacHelper txStaMac, txApMac, rxStaMac, rxApMac;
    Ssid txSsid = Ssid("ns3-ssid-tx");
    Ssid rxSsid = Ssid("ns3-ssid-rx");

    txStaMac.SetType("ns3::StaWifiMac",
                   "Ssid", SsidValue(txSsid),
                   "ActiveProbing", BooleanValue(false));
    txApMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(txSsid));

    rxStaMac.SetType("ns3::StaWifiMac",
                   "Ssid", SsidValue(rxSsid),
                   "ActiveProbing", BooleanValue(false));
    rxApMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(rxSsid));


    // Setup the PHY and set the channel
    YansWifiChannelHelper txWifiChannel = YansWifiChannelHelper::Default();
    this->txWifiPhy.SetChannel(txWifiChannel.Create());

    YansWifiChannelHelper rxWifiChannel = YansWifiChannelHelper::Default();
    this->rxWifiPhy.SetChannel(rxWifiChannel.Create());

    // Install the devices
    this->txDevices = wifi.Install(txWifiPhy, txStaMac, this->txNodes);
    this->txApDevice = wifi.Install(txWifiPhy, txApMac, this->txApNode);

    this->rxDevices = wifi.Install(rxWifiPhy, rxStaMac, this->rxNodes);
    this->rxApDevice = wifi.Install(rxWifiPhy, rxApMac, this->rxApNode);

    NS_LOG_UNCOND("AP Node installed.");

    this->p2pPhy.SetDeviceAttribute("DataRate", StringValue("128Kbps"));
    this->p2pPhy.SetChannelAttribute("Delay", StringValue("2ms"));

    this->p2pDevices = p2pPhy.Install(this->txApNode.Get(0), this->rxApNode.Get(0));
    NS_LOG_UNCOND("P2P link installed.");
}

void
Experiment::SetupIPStack()
{
    NS_LOG_UNCOND("\n");

    // Install the internet stack on the nodes
    InternetStackHelper stack;
    stack.Install(this->txNodes);
    stack.Install(this->rxNodes);
    stack.Install(this->txApNode);
    stack.Install(this->rxApNode);
    NS_LOG_UNCOND("Internet stack installed.");

    // Assign IP addresses to the devices
    Ipv4AddressHelper address;

    address.SetBase("192.168.1.0", "255.255.255.0");
    this->p2pInterfaces = address.Assign(this->p2pDevices);

    address.SetBase("192.168.2.0", "255.255.255.0");
    this->txApInterface = address.Assign(this->txApDevice);
    this->txInterfaces = address.Assign(this->txDevices);
    
    address.SetBase("192.168.3.0", "255.255.255.0");
    this->rxApInterface = address.Assign(this->rxApDevice);
    this->rxInterfaces = address.Assign(this->rxDevices);

    NS_LOG_UNCOND("IP addresses assigned.");

    // Populate routing table
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    NS_LOG_UNCOND("Routing Tables Populated.");
}

// Setup TCP congfigurations
void
Experiment::SetupTCP()
{
    // Select TCP variant
    transport_prot = std::string("ns3::") + transport_prot;
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(transport_prot, &tcpTid),
                        "TypeId " << transport_prot << " not found");
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                       TypeIdValue(TypeId::LookupByName(transport_prot)));
}

void
Experiment::SetupApplication()
{
    NS_LOG_UNCOND("\n");

    // Configure the receiver node to receive packets
    uint16_t port = 9;
    // Create a packet sink on the receiver nodes
    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = sink.Install(this->rxNodes);
    sinkApps.Start(Seconds(0));
    sinkApps.Stop(Seconds(10));
    NS_LOG_UNCOND("Packet sink installed.");

    // Create a traffic generator on the transmitter nodes
    // Install the traffic on the transmitter nodes
    // For each txNode
    int numNodes = txNodes.GetN();
    for (uint32_t i = 0; i < numNodes; i++)
    {
        OnOffHelper onoff("ns3::TcpSocketFactory",
                          InetSocketAddress(rxInterfaces.GetAddress(i), port));
        onoff.SetConstantRate(DataRate(rate * 1000), 512U);
        onoff.SetAttribute("StartTime", TimeValue(Seconds(1)));
        onoff.SetAttribute("StopTime", TimeValue(Seconds(10)));

        Ptr<Node> node = txNodes.Get(i);
        ApplicationContainer app = onoff.Install(node);

        // Print the traffic generator's address and port
        NS_LOG_UNCOND("Traffic generator " << i << " installed on node " << node->GetId() << " at " << txInterfaces.GetAddress(i) << ":" << port);
        // Print the packet sink's address and port
        NS_LOG_UNCOND("Packet sink " << i << " installed on node " << node->GetId() << " at " << rxInterfaces.GetAddress(i) << ":" << port);

        app.Start(Seconds(1));
        app.Stop(Seconds(10));
    }
    NS_LOG_UNCOND("Traffic generator installed.");
}

void
Experiment::PlaceNodes(int distance)
{
    NS_LOG_UNCOND("\n");

    // Place nodes in a circle around the receiver
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(txNodes);
    mobility.Install(rxNodes);
    mobility.Install(rxApNode);
    mobility.Install(txApNode);
    NS_LOG_UNCOND("Mobility Models Installed on all nodes.");

    /*
    AP being at fixed position and has certain range and n TX and n RX nodes are randomly moving in
    AP's range
    */

    //    Set AP's Range

    // Place nodes in star topology
    int numNodes = txNodes.GetN();
    for (uint32_t i = 0; i < numNodes; i++)
    {
        double x = -1;
        double y = -i;
        Ptr<Node> node = txNodes.Get(i);
        Ptr<ConstantPositionMobilityModel> position =
            node->GetObject<ConstantPositionMobilityModel>();
        position->SetPosition(Vector(0, 0, 0.0));
    }

    numNodes = rxNodes.GetN();
    for (uint32_t i = 0; i < numNodes; i++)
    {
        double x = 1;
        double y = i;
        Ptr<Node> node = rxNodes.Get(i);
        Ptr<ConstantPositionMobilityModel> position =
            node->GetObject<ConstantPositionMobilityModel>();
        position->SetPosition(Vector(201.0, 201.0, 201.0));
    }

    NS_LOG_UNCOND("Transmitter and Receiver Nodes placed.");

    Ptr<ConstantPositionMobilityModel> txApPosition =
        txApNode.Get(0)->GetObject<ConstantPositionMobilityModel>();
    txApPosition->SetPosition(Vector(0.0, 0.0, 0.0));

    Ptr<ConstantPositionMobilityModel> rxApPosition =
        rxApNode.Get(0)->GetObject<ConstantPositionMobilityModel>();
    rxApPosition->SetPosition(Vector(201.0, 201.0, 201.0));



    NS_LOG_UNCOND("AP Node placed.");
}

void
Experiment::Setup()
{
    // Create nodes
    this->CreateNodes();
    // Setup WiFi
    this->SetupWiFi();
    // Setup IP stack
    this->SetupIPStack();
    // Setup TCP configuration
    this->SetupTCP();
    // Setup application
    this->SetupApplication();
    // Place nodes
    this->PlaceNodes();
}

void
Experiment::PostSetup()
{
    // Add flow monitor and install trace sinks
    this->monitor = this->flowmon.InstallAll();
    Config::Connect(
        "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::WifiMac/Txop/BackoffTrace",
        MakeCallback(&Experiment::OnBackoff, this));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTxDrop",
                                  MakeCallback(&Experiment::OnCollision, this));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxEnd",
                                  MakeCallback(&Experiment::OnTx, this));
    Simulator::Stop(Seconds(1 + 10));

    this->p2pPhy.EnablePcap("p2p", p2pDevices.Get(0));
}

void
Experiment::Run()
{
    // NS_LOG_UNCOND("Running simulation.");
    Simulator::Run();
}

void
Experiment::PreTeardown()
{
    // Check for lost packets and collect stats
    this->monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    uint64_t totalRxBytes = 0;
    double firstTxTime = INT_MAX;
    double lastRxTime = 0;

    double flowCount = 0.;

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
         i != stats.end();
         ++i)
    {
        ns3::FlowMonitor::FlowStats stats = i->second;

        // Print the rx and tx node ids
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        // NS_LOG_UNCOND("Flow " << i->first << " (" << t.sourceAddress << " -> "
        //                       << t.destinationAddress << ")\n");

        totalRxBytes += stats.rxBytes;

        if (stats.timeFirstTxPacket.GetSeconds() < firstTxTime)
            firstTxTime = stats.timeFirstTxPacket.GetSeconds();

        if (stats.timeLastRxPacket.GetSeconds() > lastRxTime)
            lastRxTime = stats.timeLastRxPacket.GetSeconds();

        double txThroughput = (stats.rxBytes * 8.0) / (stats.timeLastTxPacket.GetSeconds() -
                                                       stats.timeFirstTxPacket.GetSeconds());

        this->avgTxThroughput =
            (this->avgTxThroughput * flowCount + txThroughput) / (flowCount + 1);
        flowCount++;

        // NS_LOG_UNCOND("  Tx Packets: " << stats.txPackets);
        // NS_LOG_UNCOND("  Tx Bytes:   " << stats.txBytes);
        // NS_LOG_UNCOND("  Tx Offered: " << txThroughput << " bps");
        // NS_LOG_UNCOND("  Rx Packets: " << stats.rxPackets);
        // NS_LOG_UNCOND("  Rx Bytes:   " << stats.rxBytes);
        // NS_LOG_UNCOND("  Lost Packets: " << stats.lostPackets);
        // NS_LOG_UNCOND("  Lost Ratio: " << stats.lostPackets / stats.txPackets);
        // NS_LOG_UNCOND("  Delay Sum: " << stats.delaySum);
        // NS_LOG_UNCOND("  Jitter Sum: " << stats.jitterSum);
        // NS_LOG_UNCOND("  Throughput: " << stats.rxBytes * 8.0 / (stats.timeLastRxPacket.GetSeconds() -
        //                                                          stats.timeFirstRxPacket.GetSeconds()));
        // NS_LOG_UNCOND("  Mean Delay: " << stats.delaySum.GetSeconds() / stats.rxPackets);
        // NS_LOG_UNCOND("  Mean Jitter: " << stats.jitterSum.GetSeconds() / stats.rxPackets);

        // NS_LOG_UNCOND("  Time First Tx: " << stats.timeFirstTxPacket.GetSeconds());
        // NS_LOG_UNCOND("  Time Last Tx: " << stats.timeLastTxPacket.GetSeconds());
        // NS_LOG_UNCOND("  Time First Rx: " << stats.timeFirstRxPacket.GetSeconds());
        // NS_LOG_UNCOND("  Time Last Rx: " << stats.timeLastRxPacket.GetSeconds());

        // Generate PCAP files for each node
        

    }
    // Calculate the collision rate
    this->collisionRate = ((double)this->collisionCount / (double)this->txCount) * 100;
    std::cout << "Avg. Tx Throughput: " << std::fixed << std::setprecision(2)
              << this->avgTxThroughput / 1000. << " kbps" << std::endl;
    std::cout << "Collision Rate:     " << std::fixed << std::setprecision(2) << this->collisionRate
              << "%" << std::endl;
}

void
Experiment::Teardown()
{
    Simulator::Destroy();
    NS_LOG_LOGIC("Simulation Teardown.");
}

Experiment::Result
Experiment::getResults()
{
    return Experiment::Result(this->nodes,
                              this->rate,
                              this->rxThroughput,
                              this->avgTxThroughput,
                              this->avgBackoffTime / this->txCount);
}

void
RunExperiment(Experiment& experiment)
{
    // Configure the experiment
    experiment.Setup();
    experiment.PostSetup();
    // Run the experiment
    experiment.Run();
    experiment.PreTeardown();
    // Teardown the experiment
    experiment.Teardown();
}

int
main(int argc, char* argv[])
{
    NS_LOG_UNCOND("EE595 Lab 1 NS-3 Simulation");

    CommandLine cmd(__FILE__);

    // Command line arguments
    cmd.AddValue("nodes", "Number of nodes", nodes);            // Number of nodes
    cmd.AddValue("linkDataRate", "Link Data Rate", linkDataRate); // Link Data Rate
    cmd.AddValue("rate", "Data Rate", dataRate);        // Simulation time
    cmd.AddValue("tcp_cc", "Transport protocol to use: TcpNewReno, TcpCubic, TcpMl (disabled)", transport_prot);
    cmd.Parse(argc, argv);

    NS_LOG_UNCOND("Running test with " << nodes << " nodes at " << dataRate << " Kbps");
    Experiment exp(nodes, dataRate);
    RunExperiment(exp); // Run the experiment

    return 0;
}
