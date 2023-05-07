// #ifndef TCP_RL_ENV_H
// #define TCP_RL_ENV_H

// #include "ns3/core-module.h"
// #include "ns3/network-module.h"
// #include "ns3/tcp-header.h"
// #include "ns3/tcp-socket-base.h"
// #include "ns3/ns3-ai-module.h"

// namespace ns3 {

// struct sTcpMlEsnv {
//     uint32_t nodeId;
//     uint32_t socketUid;
//     uint8_t envType;
//     int64_t simTime_us;
//     uint32_t ssThresh;
//     uint32_t cWnd;
//     uint32_t segmentSize;
//     uint32_t segmentsAcked;
//     uint32_t bytesInFlight;
//     int64_t rtt;
//     int64_t minRtt;
//     uint32_t calledFunc;
//     uint32_t congState;
//     uint32_t event;
//     uint32_t ecnState;
// } Packed;

// struct TcpMlAct {
//     uint32_t new_ssThresh;
//     uint32_t new_cWnd;
// };

// typedef enum {
//     GET_SS_THRESH = 0,
//     INCREASE_WINDOW,
//     PKTS_ACKED,
//     CONGESTION_STATE_SET,
//     CWND_EVENT,
// } CalledFunc_t;

// class TcpMlEnv : public Ns3AIRL<sTcpMlEnv, TcpMlAct> {
// public:
//   TcpMlEnv () = delete;
//   TcpMlEnv (uint16_t id);
//   void SetNodeId (uint32_t id);
//   void SetSocketUuid (uint32_t id);
//   void TxPktTrace (Ptr<const Packet>, const TcpHeader &, Ptr<const TcpSocketBase>);
//   void RxPktTrace (Ptr<const Packet>, const TcpHeader &, Ptr<const TcpSocketBase>);

//   virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight) = 0;
//   virtual void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) = 0;

//   virtual void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time &rtt) = 0;
//   virtual void CongestionStateSet (Ptr<TcpSocketState> tcb,
//                                    const TcpSocketState::TcpCongState_t newState) = 0;
//   virtual void CwndEvent (Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event) = 0;

// protected:
//   uint32_t m_nodeId;
//   uint32_t m_socketUuid;

//   bool m_isGameOver;
//   float m_envReward;

//   Time m_lastPktTxTime{MicroSeconds (0.0)};
//   Time m_lastPktRxTime{MicroSeconds (0.0)};
//   uint64_t m_interTxTimeNum{0};
//   Time m_interTxTimeSum{MicroSeconds (0.0)};
//   uint64_t m_interRxTimeNum{0};
//   Time m_interRxTimeSum{MicroSeconds (0.0)};

//   uint32_t m_new_ssThresh;
//   uint32_t m_new_cWnd;
// };

// class TcpTimeStepEnv : public TcpMlEnv {
// public:
//   TcpTimeStepEnv () = delete;
//   TcpTimeStepEnv (uint16_t id);
//   virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight);
//   virtual void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
//   virtual void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time &rtt);
//   virtual void CongestionStateSet (Ptr<TcpSocketState> tcb,
//                                    const TcpSocketState::TcpCongState_t newState);
//   virtual void CwndEvent (Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event);

// private:
//   void ScheduleNextStateRead ();
//   bool m_started{false};
//   Time m_timeStep{MilliSeconds(10)};
//   Ptr<const TcpSocketState> m_tcb;
//   std::vector<uint32_t> m_bytesInFlight;
//   std::vector<uint32_t> m_segmentsAcked;

//   uint64_t m_rttSampleNum{0};
//   Time m_rttSum{MicroSeconds (0.0)};
// };

// } // namespace ns3

// #endif // TCP_RL_ENV_H
