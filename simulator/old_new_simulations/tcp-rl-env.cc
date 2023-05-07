// #include <numeric>
// #include "tcp-rl-env.hh"
// namespace ns3 {

// NS_LOG_COMPONENT_DEFINE("ns3::TcpRlEnv");
// TcpMlEnv::TcpMlEnv(uint16_t id) : Ns3AIRL<sTcpMlEnv, TcpMlAct>(id) {
//   SetCond(2, 0);
// }

// void TcpMlEnv::SetNodeId(uint32_t id) {
//   NS_LOG_FUNCTION(this);
//   m_nodeId = id;
// }

// void TcpMlEnv::SetSocketUuid(uint32_t id) {
//   NS_LOG_FUNCTION(this);
//   m_socketUuid = id;
// }

// void TcpMlEnv::TxPktTrace(Ptr<const Packet>, const TcpHeader &, Ptr<const TcpSocketBase>) {
//   NS_LOG_FUNCTION (this);
//   if (m_lastPktTxTime > MicroSeconds(0.0)) {
//     Time interTxTime = Simulator::Now() - m_lastPktTxTime;
//     m_interTxTimeSum += interTxTime;
//     m_interTxTimeNum++;
//   }

//   m_lastPktTxTime = Simulator::Now();
// }

// void TcpMlEnv::RxPktTrace(Ptr<const Packet>, const TcpHeader &, Ptr<const TcpSocketBase>) {
//   if (m_lastPktRxTime > MicroSeconds(0.0)) {
//     Time interRxTime = Simulator::Now() - m_lastPktRxTime;
//     m_interRxTimeSum += interRxTime;
//     m_interRxTimeNum++;
//   }

//   m_lastPktRxTime = Simulator::Now();
// }


// TcpTimeStepEnv::TcpTimeStepEnv(uint16_t id) : TcpMlEnv(id) {}

// void TcpTimeStepEnv::ScheduleNextStateRead() {
//   Simulator::Schedule(m_timeStep, &TcpTimeStepEnv::ScheduleNextStateRead, this);

//   auto env = EnvSetterCond();
//   env->socketUid = m_socketUuid;
//   env->envType = 1;
//   env->simTime_us = Simulator::Now().GetMicroSeconds();
//   env->nodeId = m_nodeId;
//   env->ssThresh = m_tcb->m_ssThresh;
//   env->cWnd = m_tcb->m_cWnd;
//   env->segmentSize = m_tcb->m_segmentSize;

//   uint64_t bytesInFlightSum = std::accumulate(m_bytesInFlight.begin(), m_bytesInFlight.end(), 0);
//   env->bytesInFlight = bytesInFlightSum;
//   m_bytesInFlight.clear();

//   uint64_t segmentsAckedSum = std::accumulate(m_segmentsAcked.begin(), m_segmentsAcked.end(), 0);

//   env->segmentsAcked = segmentsAckedSum;
//   m_segmentsAcked.clear();
//   std::cerr << "At " << (uint64_t)(Simulator::Now().GetMilliSeconds()) << "ms:\n";
//   std::cerr << "\tstate --"
//             << " ssThresh=" << env->ssThresh
//             << " cWnd=" << env->cWnd
//             << " segmentSize=" << env->segmentSize
//             << " segmentAcked=" << env->segmentsAcked
//             << " bytesInFlightSum=" << bytesInFlightSum
//             << std::endl;
//   SetCompleted();
//   auto act = ActionGetterCond();
//   m_new_cWnd = act->new_cWnd;
//   m_new_ssThresh = act->new_ssThresh;
//   GetCompleted();
//   m_rttSampleNum = 0;
//   m_rttSum = MicroSeconds(0.0);

//   m_interTxTimeNum = 0;
//   m_interTxTimeSum = MicroSeconds(0.0);

//   m_interRxTimeNum = 0;
//   m_interRxTimeSum = MicroSeconds(0.0);
//     Time avgRtt = Seconds (0.0);
//     if (m_rttSampleNum)
//       avgRtt = m_rttSum / m_rttSampleNum;
//     env->rtt = avgRtt.GetMicroSeconds ();

//     env->minRtt = m_tcb->m_minRtt.GetMicroSeconds ();
//     // env->calledFunc = m_calledFunc;
//     env->congState = m_tcb->m_congState;
//     // env->event = m_event;
//     env->ecnState = m_tcb->m_ecnState;
//   std::cerr << "\taction --"
//             << " new_cWnd=" << m_new_cWnd
//             << " new_ssThresh=" << m_new_ssThresh
//             << std::endl;
// }

// uint32_t
// TcpTimeStepEnv::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight) {
//   NS_LOG_FUNCTION(this);
//   NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
//                                << " GetSsThresh, BytesInFlight: " << bytesInFlight);
//   m_tcb = tcb;
//   m_bytesInFlight.push_back(bytesInFlight);

//   if (!m_started) {
//     m_started = true;
//     ScheduleNextStateRead();
//   }

//   return m_new_ssThresh;
// }

// void TcpTimeStepEnv::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) {
//   NS_LOG_FUNCTION(this);
//   NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
//                                << " IncreaseWindow, SegmentsAcked: " << segmentsAcked);
//   m_tcb = tcb;
//   m_segmentsAcked.push_back(segmentsAcked);
//   m_bytesInFlight.push_back(tcb->m_bytesInFlight);

//   if (!m_started) {
//     m_started = true;
//     ScheduleNextStateRead();
//   }

//   tcb->m_cWnd = m_new_cWnd;
// }

// void TcpTimeStepEnv::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time &rtt) {
//   m_tcb = tcb;
//   m_rttSum += rtt;
//   m_rttSampleNum++;
// }

// void TcpTimeStepEnv::CongestionStateSet(Ptr<TcpSocketState> tcb,
//                                         const TcpSocketState::TcpCongState_t newState) {
//   m_tcb = tcb;
// }

// void TcpTimeStepEnv::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event) {
//   m_tcb = tcb;
// }

// } // namespace ns3
