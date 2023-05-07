#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <string>
#include <iostream>


#include "include/bitsery/bitsery.h"
#include "include/bitsery/adapter/buffer.h"
#include "include/bitsery/traits/vector.h"

using Buffer = std::vector<uint8_t>;

namespace ns3
{
    class PythonRpcClient
    {
    public:
        PythonRpcClient(const char *socket_path);

        Buffer call(Buffer& method_call);

        ~PythonRpcClient();

    private:
        const char *socket_path_;
        int socket_fd_;
        struct sockaddr_un address_;
    };

}