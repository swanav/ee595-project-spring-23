#include "tcp-ml-env.hh"

#include "include/bitsery/bitsery.h"
#include "include/bitsery/adapter/buffer.h"
#include "include/bitsery/traits/vector.h"

#include <vector>

#include "rpc.hh"

using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

namespace ns3 {

enum class MyEnum : uint16_t
{
  V1,
  V2,
  V3
};
struct MyStruct
{
  uint32_t i;
  MyEnum e;
  std::vector<float> fs;
};

// define how object should be serialized/deserialized
template<typename S>
void
serialize(S& s, MyStruct& o)
{
  s.value4b(o.i); // fundamental types (ints, floats, enums) of size 4b
  s.value2b(o.e);
  s.container4b(o.fs, 10); // resizable containers also requires maxSize, to
                           // make it safe from buffer-overflow attacks
}

NS_LOG_COMPONENT_DEFINE("ns3::TcpGymEnv");

PythonRpcClient* GymEnvironment::client = nullptr;

PythonRpcClient* GymEnvironment::GetRpcClient() {
  if (GymEnvironment::client != nullptr) {
    NS_LOG_UNCOND("GymEnvironment::GetRpcClient: client does not exist");
    GymEnvironment::client = new PythonRpcClient("/tmp/rl_agent.sock");
  }
  return GymEnvironment::client;
}

// std::string GymEnvironment::CallPythonFunction(std::string functionName, std::string arguments) {
//   auto client = this->GetRpcClient();
//   std::cout << "GymEnvironment::CallPythonFunction: " << functionName << " : " << arguments << std::endl;
//   std::string response = client->call(functionName, arguments);
//   std::cout << "GymEnvironment::CallPythonFunction: " << response << std::endl;
//   return response;
// }

GymEnvironment::GymEnvironment(int args) : SimpleRefCount<GymEnvironment>() {
  NS_LOG_FUNCTION(this);
  NS_LOG_UNCOND("GymEnvironment::GymEnvironment " << args);
  this->identifier = args;
}

GymEnvironment::~GymEnvironment() {}

void GymEnvironment::Dispose() {}

void GymEnvironment::IncreaseWindow(Ptr<TcpSocketState> state, uint32_t segmentsAcked) {
  // Reward the agent for increasing the window size
  NS_LOG_UNCOND("GymEnvironment::IncreaseWindow: " << segmentsAcked << " segmentsAcked");
  // std::string message = "IncreaseWindow " + std::to_string(this->identifier) + " " + std::to_string(segmentsAcked);
  // CallPythonFunction("IncreaseWindow", std::to_string(segmentsAcked));
  PythonRpcClient* client = this->GetRpcClient();
  // std::string response = client->call("IncreaseWindow");

  MyStruct data{ 8941, MyEnum::V2, { 15.0f, -8.5f, 0.045f } };
  Buffer buffer;
  auto writtenSize = bitsery::quickSerialization<OutputAdapter>(buffer, data);

  std::cout << "GymEnvironment::IncreaseWindow: " << writtenSize << std::endl;
}

uint32_t GymEnvironment::GetSsThresh(Ptr<const TcpSocketState> state, uint32_t bytesInFlight) {
  // Penalize the agent for causing a packet loss, and return the new threshold
  NS_LOG_UNCOND("GymEnvironment::GetSsThresh: " << bytesInFlight);
  // std::string message = "GetSsThresh " + std::to_string(this->identifier) + " " + std::to_string(bytesInFlight);
  // CallPythonFunction("GetSsThresh", std::to_string(bytesInFlight));
  return 0;
}

void GymEnvironment::ReportPacketsAcked(Ptr<TcpSocketState> state, uint32_t segmentsAcked, const Time& rtt) {

}

void GymEnvironment::ReportCwndEvent(Ptr<TcpSocketState> state, const TcpSocketState::TcpCAEvent_t event) {

}

} // namespace ns3