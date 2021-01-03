#include <K/IO/SocketStream.h>

#include <unistd.h>
#include <sys/socket.h>
#include <vector>
#include <K/Core/StringTools.h>
#include <K/Core/Log.h>
#include <K/IO/NetworkTools.h>

using std::shared_ptr;
using std::make_shared;
using std::to_string;
using std::mutex;
using std::unique_lock;
using std::string;
using std::vector;
using K::Core::StringTools;
using K::Core::Log;
using K::IO::NetworkTools;

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
    int fd = NetworkTools::ConnectTcp(host);
    return (fd >= 0) ? make_shared<SocketStream>(fd) : nullptr;
}

shared_ptr<SocketStream> SocketStream::ConnectToHost(uint32_t ip4Address, int port) {
    int fd = NetworkTools::ConnectTcp(ip4Address, port);
    return (fd >= 0) ? make_shared<SocketStream>(fd) : nullptr;
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
