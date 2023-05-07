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

NS_OBJECT_ENSURE_REGISTERED(TcpSocketDerived);

TypeId TcpSocketDerived::GetTypeId (void) {
  static TypeId tid = TypeId("ns3::TcpSocketDerived")
                      .SetParent<TcpSocketBase>()
                      .SetGroupName ("Internet")
                      .AddConstructor<TcpSocketDerived> ();
  return tid;
}

TypeId TcpSocketDerived::GetInstanceTypeId() const {
  return TcpSocketDerived::GetTypeId();
}

TcpSocketDerived::TcpSocketDerived(void) {}

Ptr<TcpCongestionOps> TcpSocketDerived::GetCongestionControlAlgorithm() {
  return m_congestionControl;
}

TcpSocketDerived::~TcpSocketDerived(void) {}

NS_LOG_COMPONENT_DEFINE("ns3::TcpMl");
NS_OBJECT_ENSURE_REGISTERED(TcpMl);

TcpMl::TcpMl(void) : TcpCongestionOps() {
  NS_LOG_FUNCTION(this);
  m_tcpSocket = 0;
}

TcpMl::TcpMl(const TcpMl &sock): TcpCongestionOps(sock) {
  NS_LOG_FUNCTION(this);
  m_tcpSocket = 0;
}

TcpMl::~TcpMl(void) {
  NS_LOG_FUNCTION(this);
  m_tcpSocket = 0;
}

TypeId TcpMl::GetTypeId(void) {
  static TypeId tid = TypeId("ns3::TcpMl")
                        .SetParent<TcpCongestionOps>()
                        .SetGroupName("Internet")
                        .AddConstructor<TcpMl>();
  return tid;
}

uint64_t TcpMl::GenerateUuid() {
  static uint64_t uuid = 0;
  uuid++;
  return uuid;
}

void TcpMl::CreateEnv() {
  NS_LOG_FUNCTION (this);
  env = Create<TcpMlEnv>(4321);
  env->SetSocketUuid(TcpMl::GenerateUuid());
  ConnectSocketCallbacks();
}

void TcpMl::ConnectSocketCallbacks() {
  NS_LOG_FUNCTION(this);

  bool foundSocket = false;
  for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i) {
    Ptr<Node> node = *i;
    Ptr<TcpL4Protocol> tcp = node->GetObject<TcpL4Protocol>();

    ObjectVectorValue socketVec;
    tcp->GetAttribute("SocketList", socketVec);

    for (uint32_t j = 0; j < socketVec.GetN(); j++) {
      Ptr<Object> sockObj = socketVec.Get(j);
      Ptr<TcpSocketBase> tcpSocket = DynamicCast<TcpSocketBase>(sockObj);
      if (!tcpSocket) continue;
      Ptr<TcpSocketDerived> dtcpSocket = StaticCast<TcpSocketDerived>(tcpSocket);
      Ptr<TcpCongestionOps> ca = dtcpSocket->GetCongestionControlAlgorithm();
      Ptr<TcpMl> rlCa = DynamicCast<TcpMl>(ca);
      if (rlCa == this) {
        foundSocket = true;
        m_tcpSocket = tcpSocket;
        break;
      }
    }

    if (foundSocket) {
      break;
    }
  }

  if(m_tcpSocket) {
    m_tcpSocket->TraceConnectWithoutContext("Tx", MakeCallback(&TcpMlEnv::TxPktTrace, env));
    m_tcpSocket->TraceConnectWithoutContext("Rx", MakeCallback(&TcpMlEnv::RxPktTrace, env));
    env->SetNodeId(m_tcpSocket->GetNode()->GetId());
  }
}


std::string TcpMl::GetName() const {
  return "TcpMl";
}

uint32_t TcpMl::GetSsThresh(Ptr<const TcpSocketState> state, uint32_t bytesInFlight) {
  NS_LOG_FUNCTION (this << state << bytesInFlight);
  if (!m_cbConnect) {
    m_cbConnect = true;
    CreateEnv();
  }

  uint32_t newSsThresh = 0;
  newSsThresh = env->GetSsThresh(state, bytesInFlight);
  return newSsThresh;
}

void TcpMl::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) {
  NS_LOG_FUNCTION(this << tcb << segmentsAcked);
  if (!m_cbConnect) {
    m_cbConnect = true;
    CreateEnv();
  }
  env->IncreaseWindow (tcb, segmentsAcked);
}

void TcpMl::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time &rtt) {
  NS_LOG_FUNCTION(this);
  if (!m_cbConnect) {
    m_cbConnect = true;
    CreateEnv();
  }
  env->PktsAcked(tcb, segmentsAcked, rtt);
}

void TcpMl::CongestionStateSet(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCongState_t newState) {
  NS_LOG_FUNCTION (this);
  if (!m_cbConnect) {
    m_cbConnect = true;
    CreateEnv();
  }
  env->CongestionStateSet (tcb, newState);
}

void TcpMl::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event) {
  NS_LOG_FUNCTION(this);
  if (!m_cbConnect) {
    m_cbConnect = true;
    CreateEnv();
  }
  env->CwndEvent(tcb, event);
}

Ptr<TcpCongestionOps> TcpMl::Fork() {
  return CopyObject<TcpMl> (this);
}

} // namespace ns3
