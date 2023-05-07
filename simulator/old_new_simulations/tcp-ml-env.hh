#ifndef TCP_ML_ENV_H
#define TCP_ML_ENV_H


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/tcp-header.h"
#include "ns3/tcp-socket-base.h"
#include "ns3/ns3-ai-module.h"

#include "rpc.hh"

namespace ns3 {


class GymEnvironment : public SimpleRefCount<GymEnvironment> {
public:
    GymEnvironment(int args);

    ~GymEnvironment();

    void Dispose();

    void IncreaseWindow(Ptr<TcpSocketState> state, uint32_t segmentsAcked);

    uint32_t GetSsThresh(Ptr<const TcpSocketState> state, uint32_t bytesInFlight);

    void ReportPacketsAcked(Ptr<TcpSocketState> state, uint32_t segmentsAcked, const Time& rtt);

    void ReportCwndEvent(Ptr<TcpSocketState> state, const TcpSocketState::TcpCAEvent_t event);
    
private:
    int identifier;
    static PythonRpcClient* client;
    PythonRpcClient* GetRpcClient();
    // std::string CallPythonFunction(std::string functionName, std::string arguments);
};

} // namespace ns3

#endif // TCP_ML_ENV_H
