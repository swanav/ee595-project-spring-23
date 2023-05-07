#include "tcp-ml.hh"
#include "ns3/tcp-header.h"
#include "ns3/object.h"
#include "ns3/node-list.h"
#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/tcp-socket-base.h"
#include "ns3/tcp-l4-protocol.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ns3::TcpMl");
NS_OBJECT_ENSURE_REGISTERED (TcpMl);

TypeId TcpMl::GetTypeId() {
    static TypeId tid = TypeId("ns3::TcpMl")
                        .SetParent<TcpCongestionOps>()
                        .SetGroupName("Internet")
                        .AddConstructor<TcpMl>();
    return tid;
}

TcpMl::TcpMl() : TcpCongestionOps() {
    NS_LOG_FUNCTION(this);
    NS_LOG_UNCOND("TcpMl::TcpMl");
    CreateEnv();
}

TcpMl::TcpMl(const TcpMl& sock) : TcpCongestionOps (sock) {
    NS_LOG_FUNCTION(this);
    NS_LOG_UNCOND("TcpMl::TcpMl (const TcpMl& sock)");
    CreateEnv();
}

TcpMl::~TcpMl() {
    NS_LOG_FUNCTION(this);
    DisposeEnv();
    NS_LOG_UNCOND("TcpMl::~TcpMl");
}


std::string TcpMl::GetName() const {
    return "TcpMl";
}

void TcpMl::IncreaseWindow(Ptr<TcpSocketState> state, uint32_t segmentsAcked) {
    NS_LOG_FUNCTION(this);
    // Called when a packet is acked. Time to increase the window size
    // NS_LOG_UNCOND("TcpMl::IncreaseWindow: " << segmentsAcked << " segmentsAcked");
    state->m_cWnd += segmentsAcked * state->m_segmentSize;
    // Ask the Gym environment for the new window size
    env->IncreaseWindow(state, segmentsAcked);
}

uint32_t TcpMl::GetSsThresh(Ptr<const TcpSocketState> state, uint32_t bytesInFlight) {
    NS_LOG_FUNCTION(this);
    // Called when a packet is lost. Time to reduce the window size
    // NS_LOG_UNCOND("TcpMl::GetSsThresh: " << bytesInFlight);
    uint32_t ssThresh = 0;
    // Ask the Gym environment for the new slow start threshold
    ssThresh = env->GetSsThresh(state, bytesInFlight);
    return ssThresh;
}

Ptr<TcpCongestionOps> TcpMl::Fork () {
  return CopyObject<TcpMl> (this);
}

void TcpMl::PktsAcked(Ptr<TcpSocketState> state, uint32_t segmentsAcked, const Time& rtt) {
    NS_LOG_FUNCTION(this);
    // NS_LOG_UNCOND("TcpMl::PktsAcked: " << segmentsAcked << " " << rtt.GetMicroSeconds() << "us");
    env->ReportPacketsAcked(state, segmentsAcked, rtt);
}

void TcpMl::CwndEvent(Ptr<TcpSocketState> state, const TcpSocketState::TcpCAEvent_t event) {
    NS_LOG_FUNCTION(this);
    // std::vector<std::string> eventNames = { "CA_EVENT_TX_START", "CA_EVENT_CWND_RESTART", "CA_EVENT_COMPLETE_CWR", "CA_EVENT_LOSS", "CA_EVENT_ECN_NO_CE", "CA_EVENT_ECN_IS_CE", "CA_EVENT_DELAYED_ACK", "CA_EVENT_NON_DELAYED_ACK" };
    // NS_LOG_UNCOND("TcpMl::CwndEvent: " << eventNames[event]);
    env->ReportCwndEvent(state, event);
}

void TcpMl::CreateEnv() {
  static int sockId = 0;
  if (!env) {
      NS_LOG_UNCOND("TcpMl::CreateEnv");
      // Initialize the Gym environment
      env = Create<GymEnvironment>(++sockId);
  }
}

void TcpMl::DisposeEnv() {
    if (env) {
        NS_LOG_UNCOND("TcpMl::DisposeEnv");
        env->Dispose();
        env = 0;
    }
}

} // namespace ns3
