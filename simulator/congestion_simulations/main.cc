#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/enum.h"
#include "ns3/error-model.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/tcp-header.h"
#include "ns3/traffic-control-module.h"
#include "ns3/udp-header.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TcpMlPerformance");

static std::map<uint32_t, bool> firstCwnd;                          //!< First congestion window.
static std::map<uint32_t, bool> firstSshThr;                        //!< First SlowStart threshold.
static std::map<uint32_t, bool> firstRtt;                           //!< First RTT.
static std::map<uint32_t, bool> firstRto;                           //!< First RTO.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> cWndStream;     //!< Congstion window output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> ssThreshStream; //!< SlowStart threshold output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rttStream;      //!< RTT output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rtoStream;      //!< RTO output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> nextTxStream;   //!< Next TX output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> nextRxStream;   //!< Next RX output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> inFlightStream; //!< In flight output stream.
static std::map<uint32_t, uint32_t> cWndValue;                      //!< congestion window value.
static std::map<uint32_t, uint32_t> ssThreshValue;                  //!< SlowStart threshold value.

static std::stringstream output_string;
static std::map<uint32_t, std::stringstream*> cwnd_string_stream;       //!< Congstion window output stream.

/**
 * Get the Node Id From Context.
 *
 * \param context The context.
 * \return the node ID.
 */
static uint32_t GetNodeIdFromContext(std::string context) {
    std::size_t const n1 = context.find_first_of('/', 1);
    std::size_t const n2 = context.find_first_of('/', n1 + 1);
    return std::stoul(context.substr(n1 + 1, n2 - n1 - 1));
}

/**
 * Congestion window tracer.
 *
 * \param context The context.
 * \param oldval Old value.
 * \param newval New value.
 */
static void CwndTracer(std::string context, uint32_t oldval, uint32_t newval) {
    uint32_t nodeId = GetNodeIdFromContext(context);

    if (firstCwnd[nodeId]) {
        (*cwnd_string_stream[nodeId]) << "(0.0:" << oldval <<")";
        *cWndStream[nodeId]->GetStream() << "0.0 " << oldval << std::endl;
        firstCwnd[nodeId] = false;
    }
    (*cwnd_string_stream[nodeId]) << ",(" << Simulator::Now().GetSeconds() << ":" << newval << ")";
    *cWndStream[nodeId]->GetStream() << Simulator::Now().GetSeconds() << " " << newval << std::endl;
    cWndValue[nodeId] = newval;

    if (!firstSshThr[nodeId]) {
        *ssThreshStream[nodeId]->GetStream()
            << Simulator::Now().GetSeconds() << " " << ssThreshValue[nodeId] << std::endl;
    }
}

/**
 * Slow start threshold tracer.
 *
 * \param context The context.
 * \param oldval Old value.
 * \param newval New value.
 */
static void SsThreshTracer(std::string context, uint32_t oldval, uint32_t newval) {
    uint32_t nodeId = GetNodeIdFromContext(context);

    if (firstSshThr[nodeId]) {
        *ssThreshStream[nodeId]->GetStream() << "0.0 " << oldval << std::endl;
        firstSshThr[nodeId] = false;
    }
    *ssThreshStream[nodeId]->GetStream()
        << Simulator::Now().GetSeconds() << " " << newval << std::endl;
    ssThreshValue[nodeId] = newval;

    // if (!firstCwnd[nodeId]) {
    //     *cWndStream[nodeId]->GetStream() << Simulator::Now().GetSeconds() << " " << cWndValue[nodeId] << std::endl;
    //     (*cwnd_string_stream[nodeId]) << ",(" << Simulator::Now().GetSeconds() << "," << cWndValue[nodeId] << ")";
    // }
}

/**
 * RTT tracer.
 *
 * \param context The context.
 * \param oldval Old value.
 * \param newval New value.
 */
static void
RttTracer(std::string context, Time oldval, Time newval)
{
    uint32_t nodeId = GetNodeIdFromContext(context);

    if (firstRtt[nodeId])
    {
        *rttStream[nodeId]->GetStream() << "0.0 " << oldval.GetSeconds() << std::endl;
        firstRtt[nodeId] = false;
    }
    *rttStream[nodeId]->GetStream()
        << Simulator::Now().GetSeconds() << " " << newval.GetSeconds() << std::endl;
}

/**
 * RTO tracer.
 *
 * \param context The context.
 * \param oldval Old value.
 * \param newval New value.
 */
static void
RtoTracer(std::string context, Time oldval, Time newval)
{
    uint32_t nodeId = GetNodeIdFromContext(context);

    if (firstRto[nodeId])
    {
        *rtoStream[nodeId]->GetStream() << "0.0 " << oldval.GetSeconds() << std::endl;
        firstRto[nodeId] = false;
    }
    *rtoStream[nodeId]->GetStream()
        << Simulator::Now().GetSeconds() << " " << newval.GetSeconds() << std::endl;
}

/**
 * Next TX tracer.
 *
 * \param context The context.
 * \param old Old sequence number.
 * \param nextTx Next sequence number.
 */
static void
NextTxTracer(std::string context, SequenceNumber32 old [[maybe_unused]], SequenceNumber32 nextTx)
{
    uint32_t nodeId = GetNodeIdFromContext(context);

    *nextTxStream[nodeId]->GetStream()
        << Simulator::Now().GetSeconds() << " " << nextTx << std::endl;
}

/**
 * In-flight tracer.
 *
 * \param context The context.
 * \param old Old value.
 * \param inFlight In flight value.
 */
static void
InFlightTracer(std::string context, uint32_t old [[maybe_unused]], uint32_t inFlight)
{
    uint32_t nodeId = GetNodeIdFromContext(context);

    *inFlightStream[nodeId]->GetStream()
        << Simulator::Now().GetSeconds() << " " << inFlight << std::endl;
}

/**
 * Next RX tracer.
 *
 * \param context The context.
 * \param old Old sequence number.
 * \param nextRx Next sequence number.
 */
static void
NextRxTracer(std::string context, SequenceNumber32 old [[maybe_unused]], SequenceNumber32 nextRx)
{
    uint32_t nodeId = GetNodeIdFromContext(context);

    *nextRxStream[nodeId]->GetStream()
        << Simulator::Now().GetSeconds() << " " << nextRx << std::endl;
}

/**
 * Congestion window trace connection.
 *
 * \param cwnd_tr_file_name Congestion window trace file name.
 * \param nodeId Node ID.
 */
static void
TraceCwnd(std::string cwnd_tr_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    cWndStream[nodeId] = ascii.CreateFileStream(cwnd_tr_file_name);
    cwnd_string_stream[nodeId] = new std::stringstream();
    Config::Connect("/NodeList/" + std::to_string(nodeId) +
                        "/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
                    MakeCallback(&CwndTracer));
}

/**
 * Slow start threshold trace connection.
 *
 * \param ssthresh_tr_file_name Slow start threshold trace file name.
 * \param nodeId Node ID.
 */
static void
TraceSsThresh(std::string ssthresh_tr_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    ssThreshStream[nodeId] = ascii.CreateFileStream(ssthresh_tr_file_name);
    Config::Connect("/NodeList/" + std::to_string(nodeId) +
                        "/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold",
                    MakeCallback(&SsThreshTracer));
}

/**
 * RTT trace connection.
 *
 * \param rtt_tr_file_name RTT trace file name.
 * \param nodeId Node ID.
 */
static void
TraceRtt(std::string rtt_tr_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    rttStream[nodeId] = ascii.CreateFileStream(rtt_tr_file_name);
    Config::Connect("/NodeList/" + std::to_string(nodeId) + "/$ns3::TcpL4Protocol/SocketList/0/RTT",
                    MakeCallback(&RttTracer));
}

/**
 * RTO trace connection.
 *
 * \param rto_tr_file_name RTO trace file name.
 * \param nodeId Node ID.
 */
static void
TraceRto(std::string rto_tr_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    rtoStream[nodeId] = ascii.CreateFileStream(rto_tr_file_name);
    Config::Connect("/NodeList/" + std::to_string(nodeId) + "/$ns3::TcpL4Protocol/SocketList/0/RTO",
                    MakeCallback(&RtoTracer));
}

/**
 * Next TX trace connection.
 *
 * \param next_tx_seq_file_name Next TX trace file name.
 * \param nodeId Node ID.
 */
static void
TraceNextTx(std::string& next_tx_seq_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    nextTxStream[nodeId] = ascii.CreateFileStream(next_tx_seq_file_name);
    Config::Connect("/NodeList/" + std::to_string(nodeId) +
                        "/$ns3::TcpL4Protocol/SocketList/0/NextTxSequence",
                    MakeCallback(&NextTxTracer));
}

/**
 * In flight trace connection.
 *
 * \param in_flight_file_name In flight trace file name.
 * \param nodeId Node ID.
 */
static void
TraceInFlight(std::string& in_flight_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    inFlightStream[nodeId] = ascii.CreateFileStream(in_flight_file_name);
    Config::Connect("/NodeList/" + std::to_string(nodeId) +
                        "/$ns3::TcpL4Protocol/SocketList/0/BytesInFlight",
                    MakeCallback(&InFlightTracer));
}

/**
 * Next RX trace connection.
 *
 * \param next_rx_seq_file_name Next RX trace file name.
 * \param nodeId Node ID.
 */
static void
TraceNextRx(std::string& next_rx_seq_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    nextRxStream[nodeId] = ascii.CreateFileStream(next_rx_seq_file_name);
    Config::Connect("/NodeList/" + std::to_string(nodeId) +
                        "/$ns3::TcpL4Protocol/SocketList/1/RxBuffer/NextRxSequence",
                    MakeCallback(&NextRxTracer));
}

static void calculateAndLogThroughput(std::map<FlowId, FlowMonitor::FlowStats> stats, std::string prefix_file_name) {

        output_string << "----------------------------------------------------------" << std::endl;
        output_string << "Simulation Results: " << std::endl;

        std::ofstream throughputFile(prefix_file_name + "/throughput.data");

        std::map<int, double> throughput;

        double totalThroughput = 0;

        output_string << "Throughput (Kbps): ";

        // Count the number of packets lost by the network
        int totalPacketsLost = 0;

        for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i) {
            double flowThroughput = i->second.rxBytes * 8.0 /
                (i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ()) /
                1024;
            throughput[i->first] = flowThroughput;
            throughputFile << flowThroughput << std::endl;
            totalThroughput += flowThroughput;
            totalPacketsLost += i->second.lostPackets;
            output_string << flowThroughput << ",";
        }
        throughputFile.close();

        output_string << std::endl;

        output_string << "Total Throughput (Kbps): " << std::fixed << std::setw(10) << totalThroughput << std::endl;
        output_string << "Lost Packet Count: " << totalPacketsLost << std::endl;
        output_string << "----------------------------------------------------------" << std::endl;
        std::cout << totalThroughput << "," << totalPacketsLost << std::endl;
}

void logCwnd() {
    output_string << "----------------------------------------------------------" << std::endl;
    output_string << "Congestion Window Progression: " << std::endl;
    for (auto it = cwnd_string_stream.begin(); it != cwnd_string_stream.end(); ++it) {
        output_string << "Node " << (it->first) << " - ";
        output_string << (*it->second).str() << std::endl;
    }
    output_string << "----------------------------------------------------------" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string transport_prot = "TcpCubic";
    double error_p = 0.05;
    std::string bandwidth = "1Mbps";
    std::string delay = "0.01ms";
    std::string access_bandwidth = "10Mbps";
    std::string access_delay = "45ms";
    bool tracing = false;
    std::string prefix_file_name = "out";
    uint64_t data_kbytes = 10;
    uint32_t mtu_bytes = 400;
    uint16_t num_flows = 1;
    double duration = 100.0;
    uint32_t run = 0;
    bool flow_monitor = false;

    // Set Environment variables - NS_GLOBAL_VALUE=SharedMemoryKey=4321;SharedMemoryPoolSize=4096;
    // NS_GLOBAL_VALUE=SharedMemoryKey=4321;SharedMemoryPoolSize=4096;
    // setenv("NS_GLOBAL_VALUE", "SharedMemoryKey=4321;SharedMemoryPoolSize=4096", 1);

    CommandLine cmd(__FILE__);
    cmd.AddValue("transport_prot", "Transport protocol to use: "
                 "TcpNewReno, TcpLinuxReno, TcpVegas, TcpCubic, TcpMl", transport_prot);

    cmd.AddValue("num_flows", "Number of flows", num_flows);
    cmd.AddValue("run", "Run index (for setting repeatable seeds)", run);

    cmd.AddValue("error_p", "Packet error rate", error_p);
    cmd.AddValue("bandwidth", "Bottleneck bandwidth", bandwidth);
    cmd.AddValue("delay", "Bottleneck delay", delay);
    cmd.AddValue("access_bandwidth", "Access link bandwidth", access_bandwidth);
    cmd.AddValue("access_delay", "Access link delay", access_delay);

    cmd.AddValue("data", "Number of Kilobytes of data to transmit", data_kbytes);
    cmd.AddValue("mtu", "Size of IP packets to send in bytes", mtu_bytes);
    cmd.AddValue("duration", "Time to allow flows to run in seconds", duration);

    cmd.AddValue("prefix_name", "Prefix of output trace file", prefix_file_name);

    cmd.AddValue("tracing", "Flag to enable/disable tracing", tracing);
    cmd.AddValue("flow_monitor", "Enable flow monitor", flow_monitor);

    cmd.Parse(argc, argv);

    transport_prot = std::string("ns3::") + transport_prot;

    SeedManager::SetSeed(1);
    SeedManager::SetRun(run);

    output_string << "============================================================" << std::endl;

    // Calculate the ADU size
    Header* temp_header = new Ipv4Header();
    uint32_t ip_header = temp_header->GetSerializedSize();
    NS_LOG_LOGIC("IP Header size is: " << ip_header);
    delete temp_header;
    temp_header = new TcpHeader();
    uint32_t tcp_header = temp_header->GetSerializedSize();
    NS_LOG_LOGIC("TCP Header size is: " << tcp_header);
    delete temp_header;
    uint32_t tcp_adu_size = mtu_bytes - 20 - (ip_header + tcp_header);
    NS_LOG_LOGIC("TCP ADU size is: " << tcp_adu_size);

    // Set the simulation start and stop time
    double start_time = 0.1;
    double stop_time = start_time + duration;

    // 2 MB of TCP buffer
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(1 << 21));
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(1 << 21));

    // Select TCP variant
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(transport_prot, &tcpTid), "TypeId " << transport_prot << " not found");
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TypeId::LookupByName(transport_prot)));

    output_string << "----------------------------------------------------------" << std::endl;
    output_string << "Execution Parameters: " << std::endl;
    output_string << "Congestion Control Protocol: " << transport_prot << std::endl;
    output_string << "Number of flows: " << num_flows << std::endl;
    output_string << "Packet error rate: " << error_p << std::endl;
    output_string << "Bottleneck bandwidth: " << bandwidth << std::endl;
    output_string << "Bottleneck delay: " << delay << std::endl;
    output_string << "Access link bandwidth: " << access_bandwidth << std::endl;
    output_string << "Access link delay: " << access_delay << std::endl;
    output_string << "Data to transmit: " << data_kbytes << " KB" << std::endl;
    output_string << "MTU: " << mtu_bytes << " bytes" << std::endl;
    output_string << "Duration: " << duration << " seconds" << std::endl;
    output_string << "Prefix of output trace file: " << prefix_file_name << std::endl;
    output_string << "Tracing: " << (tracing ? "Enabled" : "Disabled") << std::endl;
    output_string << "Flow monitor: " << (flow_monitor ? "Enabled" : "Disabled") << std::endl;
    output_string << "----------------------------------------------------------" << std::endl;

    // Create gateways, sources, and sinks
    NodeContainer gateways;
    NodeContainer sources;
    NodeContainer sinks;

    gateways.Create(1);
    sources.Create(num_flows);
    sinks.Create(num_flows);

    // Configure the error model
    // Here we use RateErrorModel with packet error rate
    Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
    uv->SetStream(50);
    RateErrorModel error_model;
    error_model.SetRandomVariable(uv);
    error_model.SetUnit(RateErrorModel::ERROR_UNIT_PACKET);
    error_model.SetRate(error_p);

    PointToPointHelper UnReLink;
    UnReLink.SetDeviceAttribute("DataRate", StringValue(bandwidth));
    UnReLink.SetChannelAttribute("Delay", StringValue(delay));
    UnReLink.SetDeviceAttribute("ReceiveErrorModel", PointerValue(&error_model));

    InternetStackHelper stack;
    stack.InstallAll();

    TrafficControlHelper tchPfifo;
    tchPfifo.SetRootQueueDisc("ns3::PfifoFastQueueDisc");

    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.255.255.0");

    // Configure the sources and sinks net devices
    // and the channels between the sources/sinks and the gateways
    PointToPointHelper LocalLink;
    LocalLink.SetDeviceAttribute("DataRate", StringValue(access_bandwidth));
    LocalLink.SetChannelAttribute("Delay", StringValue(access_delay));

    Ipv4InterfaceContainer sink_interfaces;

    DataRate access_b(access_bandwidth);
    DataRate bottle_b(bandwidth);
    Time access_d(access_delay);
    Time bottle_d(delay);

    uint32_t size = static_cast<uint32_t>((std::min(access_b, bottle_b).GetBitRate() / 8) *
                                          ((access_d + bottle_d) * 2).GetSeconds());

    Config::SetDefault("ns3::PfifoFastQueueDisc::MaxSize", QueueSizeValue(QueueSize(QueueSizeUnit::PACKETS, size / mtu_bytes)));
    Config::SetDefault("ns3::CoDelQueueDisc::MaxSize", QueueSizeValue(QueueSize(QueueSizeUnit::BYTES, size)));

    for (uint32_t i = 0; i < num_flows; i++) {
        NetDeviceContainer devices;
        devices = LocalLink.Install(sources.Get(i), gateways.Get(0));
        tchPfifo.Install(devices);
        address.NewNetwork();
        Ipv4InterfaceContainer interfaces = address.Assign(devices);

        devices = UnReLink.Install(gateways.Get(0), sinks.Get(i));
        tchPfifo.Install(devices);
        address.NewNetwork();
        interfaces = address.Assign(devices);
        sink_interfaces.Add(interfaces.Get(1));
    }

    NS_LOG_INFO("Initialize Global Routing.");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    uint16_t port = 50000;
    Address sinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port));
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkLocalAddress);

    for (uint32_t i = 0; i < sources.GetN(); i++) {
        AddressValue remoteAddress(InetSocketAddress(sink_interfaces.GetAddress(i, 0), port));
        Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(tcp_adu_size));
        BulkSendHelper ftp("ns3::TcpSocketFactory", Address());
        ftp.SetAttribute("Remote", remoteAddress);
        ftp.SetAttribute("SendSize", UintegerValue(tcp_adu_size));
        ftp.SetAttribute("MaxBytes", UintegerValue(data_kbytes * 1000));

        ApplicationContainer sourceApp = ftp.Install(sources.Get(i));
        sourceApp.Start(Seconds(start_time));
        sourceApp.Stop(Seconds(stop_time));

        sinkHelper.SetAttribute("Protocol", TypeIdValue(TcpSocketFactory::GetTypeId()));
        ApplicationContainer sinkApp = sinkHelper.Install(sinks.Get(i));
        sinkApp.Start(Seconds(start_time));
        sinkApp.Stop(Seconds(stop_time));
    }

    // Set up tracing if enabled
    if (tracing) {
        std::ofstream ascii;
        Ptr<OutputStreamWrapper> ascii_wrap;
        ascii.open(prefix_file_name + "/ascii");
        ascii_wrap = new OutputStreamWrapper(prefix_file_name + "/ascii", std::ios::out);
        stack.EnableAsciiIpv4All(ascii_wrap);

        for (uint16_t index = 0; index < num_flows; index++) {
            std::string flowString;
            flowString = "-flow" + std::to_string(index);

            firstCwnd[index + 1] = true;
            firstSshThr[index + 1] = true;
            firstRtt[index + 1] = true;
            firstRto[index + 1] = true;

            Simulator::Schedule(Seconds(start_time + 0.00001),
                                &TraceCwnd,
                                prefix_file_name + flowString + "/cwnd.data",
                                index + 1);
            Simulator::Schedule(Seconds(start_time + 0.00001),
                                &TraceSsThresh,
                                prefix_file_name + flowString + "/ssth.data",
                                index + 1);
            Simulator::Schedule(Seconds(start_time + 0.00001),
                                &TraceRtt,
                                prefix_file_name + flowString + "/rtt.data",
                                index + 1);
            Simulator::Schedule(Seconds(start_time + 0.00001),
                                &TraceRto,
                                prefix_file_name + flowString + "/rto.data",
                                index + 1);
            Simulator::Schedule(Seconds(start_time + 0.00001),
                                &TraceNextTx,
                                prefix_file_name + flowString + "/next-tx.data",
                                index + 1);
            Simulator::Schedule(Seconds(start_time + 0.00001),
                                &TraceInFlight,
                                prefix_file_name + flowString + "/inflight.data",
                                index + 1);
            Simulator::Schedule(Seconds(start_time + 0.1),
                                &TraceNextRx,
                                prefix_file_name + flowString + "/next-rx.data",
                                num_flows + index + 1);
        }
    }

    // Flow monitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> flowMonitor;
    if (flow_monitor) {
        flowMonitor = flowHelper.InstallAll();
    }
    Simulator::Stop(Seconds(stop_time));
    Simulator::Run();
    if (tracing) {
        logCwnd();
    }
    if (flow_monitor) {
        flowMonitor->CheckForLostPackets();
        flowHelper.SerializeToXmlFile(prefix_file_name + "/fl.flowmonitor", true, true);
        std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
        calculateAndLogThroughput(stats, prefix_file_name);
    }
    Simulator::Destroy();
    output_string << "============================================================" << std::endl;
    // std::cout << output_string.str();
    return 0;
}
