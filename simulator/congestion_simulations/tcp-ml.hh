#ifndef TCP_ML_H
#define TCP_ML_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-socket-base.h"
#include "tcp-ml-env.hh"

namespace ns3 {

class TcpSocketBase;
class Time;

// used to get pointer to Congestion Algorithm
class TcpSocketDerived : public TcpSocketBase {
public:
  static TypeId GetTypeId(void);
  virtual TypeId GetInstanceTypeId() const;

  TcpSocketDerived(void);
  virtual ~TcpSocketDerived(void);

  Ptr<TcpCongestionOps> GetCongestionControlAlgorithm();
};


class TcpMl: public TcpCongestionOps {
public:
  static TypeId GetTypeId(void);

  TcpMl();
  TcpMl(const TcpMl &sock);
  ~TcpMl();

  virtual std::string GetName() const;

  virtual uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight);
  virtual void IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
  virtual void PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time &rtt);
  virtual void CongestionStateSet(Ptr<TcpSocketState> tcb,
                                   const TcpSocketState::TcpCongState_t newState);
  virtual void CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event);
  virtual Ptr<TcpCongestionOps> Fork();
  // virtual void ReduceCwnd(Ptr<TcpSocketState> tcb);
protected:
  static uint64_t GenerateUuid();
  virtual void CreateEnv();
  void ConnectSocketCallbacks();

  bool m_cbConnect{false};

  Ptr<TcpSocketBase> m_tcpSocket{0};

  Ptr<TcpMlEnv> env;
};

} // namespace ns3

#endif /* TCP_ML_H */