#include <K/IO/NetworkTools.h>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <cerrno>
#include <K/Core/config.h>
#include <K/Core/Interface.h>
#include <K/Core/StringTools.h>
#include <K/Core/Log.h>

using std::nullopt;
using std::optional;
using std::string;
using std::to_string;
using std::vector;
using K::Core::StringTools;
using K::Core::Log;

namespace K {
namespace IO {

optional<int> NetworkTools::ConnectTcp(const string &hostAndPort, Core::Interface *loggingObject) {
    vector<string> tokens = StringTools::Tokenize(hostAndPort, ":", false);
    if (tokens.size() == 2) {
        int port;
        if (StringTools::Parse(tokens[1], &port)) {
            return ConnectTcp(tokens[0], port, loggingObject);
        }
    }

    return nullopt;
}

optional<int> NetworkTools::ConnectTcp(const string &host, int port, Core::Interface *loggingObject) {
    uint32_t ip4Address;
    if (ResolveHostName(host, &ip4Address, loggingObject)) {
        return ConnectTcp(ip4Address, port, loggingObject);
    }

    return nullopt;
}

optional<int> NetworkTools::ConnectTcp(uint32_t ip4Address, int port, Core::Interface *loggingObject) {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd != -1) {
        if (PrepareSocket(fd, loggingObject)) {
            struct sockaddr_in address = {};
            address.sin_family      = AF_INET;
            address.sin_addr.s_addr = htonl(ip4Address);
            address.sin_port        = htons(static_cast<uint16_t>(port));
            while (true) {
                if (!connect(fd, (struct sockaddr *)&address, sizeof(address))) {
                    Log::Print(Log::Level::Debug, loggingObject, [&]{ return "TCP socket " + to_string(fd)
                        + " connected to ip4=" + Ip4ToString(ip4Address) + ", port=" + to_string(port); });
                    return fd;
                }
                else if (errno == EINTR) {
                    continue;
                }
                else {
                    Log::Print(Log::Level::Warning, loggingObject, [&]{ return "failed to connect TCP socket "
                        + to_string(fd) + " to ip4=" + Ip4ToString(ip4Address) + ", port=" + to_string(port); });
                    break;
                }
            }
        }
        close(fd);
    }
    else {
        Log::Print(Log::Level::Warning, loggingObject, []{ return "failed to create socket"; });
    }

    return nullopt;
}

bool NetworkTools::ResolveHostName(const string &hostName, uint32_t *outIp4Address, Core::Interface *loggingObject) {
    Log::Print(Log::Level::Debug, loggingObject, [&]{ return "resolving host name \"" + hostName + "\"..."; });

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
            Log::Print(Log::Level::Debug, loggingObject, [&]{ return "resolved host name \"" + hostName + "\" to "
                + Ip4ToString(ip); });
            *outIp4Address = ip;
            return true;
        }
    }

    Log::Print(Log::Level::Warning, loggingObject, [&]{ return "failed to resolve host name \"" + hostName + "\""; });
    return false;
}

string NetworkTools::Ip4ToString(uint32_t ip4Address) {
    uint8_t *ip = reinterpret_cast<uint8_t *>(&ip4Address);
    return to_string(ip[3]) + "." + to_string(ip[2]) + "." + to_string(ip[1]) + "." + to_string(ip[0]);
}

bool NetworkTools::PrepareSocket(int fd, Core::Interface *loggingObject) {
    static_assert(K_PLATFORM_SET);
#if defined(K_PLATFORM_APPLE)
    int enabled = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &enabled, sizeof(enabled))) {
        Log::Print(Log::Level::Warning, loggingObject, [&]{
            return "failed to disable SIGPIPE for socket " + to_string(fd); });
        return false;
    }
    return true;
#else
    (void)fd;
    (void)loggingObject;
    return true;
#endif
}

}    // Namespace IO.
}    // namespace K.
