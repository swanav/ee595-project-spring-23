#include <numeric>
#include "tcp-ml-env.hh"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ns3::TcpMlEnv");

TcpMlEnv::TcpMlEnv(uint16_t id) : Ns3AIRL<TcpMlEnvironment, TcpMlActions>(id) {
  SetCond(2, 0);
}

void TcpMlEnv::SetNodeId(uint32_t id) {
  NS_LOG_FUNCTION(this);
  m_nodeId = id;
}

void TcpMlEnv::SetSocketUuid(uint32_t id) {
  NS_LOG_FUNCTION(this);
  m_socketUuid = id;
}

void TcpMlEnv::TxPktTrace(Ptr<const Packet> packet, const TcpHeader& header, Ptr<const TcpSocketBase> socket) {
  NS_LOG_FUNCTION (this);
  if (m_lastPktTxTime > MicroSeconds(0.0)) {
    Time interTxTime = Simulator::Now() - m_lastPktTxTime;
    m_interTxTimeSum += interTxTime;
    m_interTxTimeNum++;
  }
  m_lastPktTxTime = Simulator::Now();
}

void TcpMlEnv::RxPktTrace(Ptr<const Packet> packet, const TcpHeader & header, Ptr<const TcpSocketBase> socket) {
  NS_LOG_FUNCTION (this);
  if (m_lastPktRxTime > MicroSeconds(0.0)) {
    Time interRxTime = Simulator::Now() - m_lastPktRxTime;
    m_interRxTimeSum += interRxTime;
    m_interRxTimeNum++;
  }
  m_lastPktRxTime = Simulator::Now();
}

void TcpMlEnv::StateRead() {
  auto env = EnvSetterCond();
  env->socketUid = m_socketUuid;
  env->envType = 1;
  env->simTime_us = Simulator::Now().GetMicroSeconds();
  env->nodeId = m_nodeId;
  env->ssThresh = m_tcb->m_ssThresh;
  env->cwnd = m_tcb->m_cWnd;
  env->segmentSize = m_tcb->m_segmentSize;
  env->bytesInFlight = m_bytesInFlight;
  env->segmentsAcked = m_segmentsAcked;
  m_bytesInFlight = 0;
  m_segmentsAcked = 0;
  SetCompleted();
  // std::cerr << "At " << (uint64_t)(Simulator::Now().GetMilliSeconds()) << "ms:\n";
  // std::cerr << "\tstate -- ssThresh=" << env->ssThresh
  //           << " cwnd=" << env->cwnd
  //           << " segmentSize=" << env->segmentSize
  //           << " segmentAcked=" << env->segmentsAcked
  //           << " bytesInFlightSum=" << env->bytesInFlight
  //           << " rtt=" << env->rtt
  //           << std::endl;

  auto act = ActionGetterCond();
  m_new_cwnd = act->new_cWnd;
  m_new_ssThresh = act->new_ssThresh;
  GetCompleted();

  // std::cerr << "\taction -- " 
  //           << "new_cwnd=" << m_new_cwnd 
  //           << " new_ssThresh=" << m_new_ssThresh 
  //           << std::endl;
}

uint32_t TcpMlEnv::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight) {
  NS_LOG_FUNCTION(this);
  // NS_LOG_UNCOND(Simulator::Now() << " Node: " << m_nodeId
  //                              << " GetSsThresh, BytesInFlight: " << bytesInFlight);
  m_tcb = tcb;
  m_bytesInFlight += bytesInFlight;
  if (!m_started) {
    m_started = true;
  }

  StateRead();
  return m_new_ssThresh;
}

void TcpMlEnv::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) {
  NS_LOG_FUNCTION(this);
  // NS_LOG_UNCOND(Simulator::Now() << " Node: " << m_nodeId << " IncreaseWindow, SegmentsAcked: " << segmentsAcked);
  m_tcb = tcb;
  m_segmentsAcked += segmentsAcked;
  m_bytesInFlight += tcb->m_bytesInFlight;

  if (!m_started) {
    m_started = true;
  }

  StateRead();
  tcb->m_cWnd = m_new_cwnd;
}

void TcpMlEnv::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time &rtt) {
  NS_LOG_FUNCTION(this);
  m_tcb = tcb;
}

void TcpMlEnv::CongestionStateSet(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCongState_t newState) {
  NS_LOG_FUNCTION(this);
  m_tcb = tcb;
}

void TcpMlEnv::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event) {
  NS_LOG_FUNCTION(this);
  m_tcb = tcb;
}

} // namespace ns3