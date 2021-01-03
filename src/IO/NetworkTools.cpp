#include <K/IO/NetworkTools.h>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <cerrno>
#include <K/Core/StringTools.h>
#include <K/Core/Log.h>

using std::string;
using std::to_string;
using std::vector;
using K::Core::StringTools;
using K::Core::Log;

namespace K {
namespace IO {

int NetworkTools::ConnectTcp(const string &host) {
    vector<string> tokens;
    StringTools::Tokenize(host, ':', &tokens);
    if (tokens.size() == 2) {
        int port;
        if (StringTools::Parse(tokens[1], &port)) {
            Log::Print(Log::Level::Debug, nullptr, [=]{ return "port is " + to_string(port); });
            uint32_t ip4Address;
            if (ResolveHostName(tokens[0], &ip4Address)) {
                return ConnectTcp(ip4Address, port);
            }
        }
    }

    return -1;
}

int NetworkTools::ConnectTcp(uint32_t ip4Address, int port) {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd != -1) {
        struct sockaddr_in address = {};
        address.sin_family      = AF_INET;
        address.sin_addr.s_addr = htonl(ip4Address);
        address.sin_port        = htons(static_cast<uint16_t>(port));
        while (true) {
            if (!connect(fd, (struct sockaddr *)&address, sizeof(address))) {
                Log::Print(Log::Level::Debug, nullptr, [=]{ return "socket " + to_string(fd) + " connected to ip4="
                    + Ip4ToString(ip4Address) + ", port=" + to_string(port); });
                return fd;
            }
            else if (errno == EINTR) {
                continue;
            }
            else {
                break;
            }
        }
        close(fd);
    }

    return -1;
}

bool NetworkTools::ResolveHostName(const string &hostName, uint32_t *outIp4Address) {
    Log::Print(Log::Level::Debug, nullptr, [=]{ return "resolving host name \"" + hostName + "\"..."; });

    struct addrinfo *result = nullptr;
    struct addrinfo hints   = { };
    hints.ai_family = PF_INET;
    if (!getaddrinfo(hostName.c_str(), nullptr, &hints, &result)) {
        uint32_t ip;
        struct addrinfo *current = result;
        while(current) {
            if(current->ai_family == PF_INET) {
              ip = ntohl(((struct sockaddr_in *)(current->ai_addr))->sin_addr.s_addr);
              break;
           }
           current = current->ai_next;
        }
        freeaddrinfo(result);

        if (current) {
            *outIp4Address = ip;
            return true;
        }
    }

    return false;
}

string NetworkTools::Ip4ToString(uint32_t ip4Address) {
    uint8_t *ip = reinterpret_cast<uint8_t *>(&ip4Address);
    return to_string(ip[3]) + "." + to_string(ip[2]) + "." + to_string(ip[1]) + "." + to_string(ip[0]);
}

}    // Namespace IO.
}    // namespace K.
