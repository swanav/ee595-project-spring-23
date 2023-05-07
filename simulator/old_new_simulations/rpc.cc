#include "rpc.hh"

namespace ns3 {

PythonRpcClient::PythonRpcClient(const char *socket_path) : socket_path_(socket_path) {
    socket_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd_ == -1)
    {
        throw std::runtime_error("Error creating client socket");
    }

    memset(&address_, 0, sizeof(address_));
    address_.sun_family = AF_UNIX;
    strncpy(address_.sun_path, socket_path_, sizeof(address_.sun_path) - 1);

    if (connect(socket_fd_, (struct sockaddr *)&address_, sizeof(address_)) == -1)
    {
        close(socket_fd_);
        throw std::runtime_error("Error connecting to server");
    }
}

PythonRpcClient::~PythonRpcClient()
{
    close(socket_fd_);
}

Buffer PythonRpcClient::call(Buffer& method_call) {
    std::cout << "PythonRpcClient::call: " << method_call.size() << " bytes" << std::endl;

    ssize_t num_bytes_sent = send(socket_fd_, method_call.data(), method_call.size(), 0);
    if (num_bytes_sent == -1)
    {
        throw std::runtime_error("Error sending data to server");
    }

    char buffer[256];
    ssize_t num_bytes_received = recv(socket_fd_, buffer, sizeof(buffer), 0);
    if (num_bytes_received == -1)
    {
        throw std::runtime_error("Error receiving data from server");
    }

    Buffer response(buffer, buffer + num_bytes_received);

    return response;
}

} // namespace ns3