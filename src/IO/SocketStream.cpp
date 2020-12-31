#include <K/IO/SocketStream.h>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <vector>
#include <K/Core/StringTools.h>
#include <K/Core/Log.h>

using std::shared_ptr;
using std::make_shared;
using std::to_string;
using std::mutex;
using std::unique_lock;
using std::string;
using std::vector;
using K::Core::StringTools;
using K::Core::Log;

namespace K {
namespace IO {

SocketStream::SocketStream(int fd)
        : socketDown_(false),
          eof_(false),
          error_(false) {
    Log::Print(Log::Level::Debug, this, [=]{ return "fd=" + to_string(fd); });
    if (fd < 0) {
        fd          = -1;
        socketDown_ = true;
        error_      = true;
    }

    fd_ = fd;
}

SocketStream::~SocketStream() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    ShutDownSocket();
    if (fd_ != -1) {
        close(fd_);
        Log::Print(Log::Level::Debug, this, [=]{ return "socket " + to_string(fd_) + " closed"; });
    }
}    // ......................................................................................... critical section, end.

void SocketStream::ShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    ShutDownSocket();
    if (!error_) {
        eof_ = true;
    }
}    // ......................................................................................... critical section, end.

int SocketStream::Read(void *outBuffer, int bufferSize) {
    int fd;
    {
        unique_lock<mutex> critical(lock_);    // Critical section......................................................
        if (socketDown_) {
            return 0;
        }
        fd = fd_;
    }    // ..................................................................................... critical section, end.

    while (true) {
        int num = read(fd, outBuffer, bufferSize);
        if (num == 0) {
            unique_lock<mutex> critical(lock_);    // Critical section..................................................
            ShutDownSocket();
            eof_ = true;
            return 0;
        }    // ................................................................................. critical section, end.
        else if (num == -1) {
            if (errno == EINTR) {
                continue;
            }
            else {
                unique_lock<mutex> critical(lock_);    // Critical section..............................................
                ShutDownSocket();
                error_ = true;
                return 0;
            }    // ............................................................................. critical section, end.
        }
        else {
            return num;
        }
    }
}

int SocketStream::Write(const void *data, int dataSize) {
    int fd;
    {
        unique_lock<mutex> critical(lock_);    // Critical section......................................................
        if (socketDown_) {
            error_ = true;
            return 0;
        }
        fd = fd_;
    }    // ..................................................................................... critical section, end.

    while (true) {
        int num = write(fd, data, dataSize);
        if (num == -1) {
            if (errno == EINTR) {
                continue;
            }
            else {
                unique_lock<mutex> critical(lock_);    // Critical section..............................................
                ShutDownSocket();
                error_ = true;
                return 0;
            }    // ............................................................................. critical section, end.
        }
        else {
            return num;
        }
    }
}

bool SocketStream::Eof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return eof_;
}    // ......................................................................................... critical section, end.

bool SocketStream::ErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return error_;
}    // ......................................................................................... critical section, end.

shared_ptr<SocketStream> SocketStream::ConnectToHost(const string &host) {
    vector<string> tokens;
    StringTools::Tokenize(host, ':', &tokens);
    if (tokens.size() == 2) {
        int port;
        if (StringTools::Parse(tokens[1], &port)) {
            Log::Print(Log::Level::Debug, nullptr, [=]{ return "port is " + to_string(port); });
            uint32_t ip4Address;
            if (ResolveHostName(tokens[0], &ip4Address)) {
                return ConnectToHost(ip4Address, port);
            }
        }
    }

    return nullptr;
}

shared_ptr<SocketStream> SocketStream::ConnectToHost(uint32_t ip4Address, int port) {
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
                return make_shared<SocketStream>(fd);
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

    return nullptr;
}

bool SocketStream::ResolveHostName(const string &hostName, uint32_t *outIp4Address) {
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

string SocketStream::Ip4ToString(uint32_t ip4Address) {
    uint8_t *ip = reinterpret_cast<uint8_t *>(&ip4Address);
    return to_string(ip[3]) + "." + to_string(ip[2]) + "." + to_string(ip[1]) + "." + to_string(ip[0]);
}

// Expects the lock to be held.
void SocketStream::ShutDownSocket() {
    if (fd_ != -1) {
        if (!socketDown_) {
            if (!shutdown(fd_, SHUT_RDWR)) {
                error_ = true;
            }
            socketDown_ = true;
            Log::Print(Log::Level::Debug, this, [=]{ return "socket " + to_string(fd_) + " shut down"; });
        }
    }
}

}    // Namesapce IO.
}    // Namespace K.
