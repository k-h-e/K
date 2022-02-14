#include <K/IO/Socket.h>

#include <unistd.h>
#include <sys/socket.h>
#include <vector>
#include <K/Core/StringTools.h>
#include <K/Core/Result.h>
#include <K/Core/Log.h>
#include <K/IO/IOTools.h>
#include <K/IO/NetworkTools.h>

using std::shared_ptr;
using std::make_shared;
using std::to_string;
using std::mutex;
using std::unique_lock;
using std::string;
using std::vector;
using K::Core::Result;
using K::Core::StringTools;
using K::Core::Log;
using K::IO::IOTools;
using K::IO::NetworkTools;

namespace K {
namespace IO {

Socket::Socket(int fd) : Socket(fd, nullptr) {}

Socket::Socket(int fd, const shared_ptr<Result> &resultAcceptor)
        : socketDown_(false),
          eof_(false),
          error_(false),
          finalResultAcceptor_(resultAcceptor) {
    Log::Print(Log::Level::Debug, this, [=]{ return "fd=" + to_string(fd); });
    if (fd < 0) {
        fd          = -1;
        socketDown_ = true;
        error_      = true;
    }

    fd_ = fd;
}

Socket::~Socket() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    ShutDownSocket();
    if (fd_ != -1) {
        if (!IOTools::CloseFileDescriptor(fd_, this)) {
            error_ = true;
        }
    }

    Result finalResult(!error_);
    if (finalResultAcceptor_) {
        *finalResultAcceptor_ = finalResult;
    }
    Log::Print(Log::Level::Debug, this, [&]{
        return "closed, fd=" + to_string(fd_) + ", final_result=" + finalResult.ToString();
    });
}    // ......................................................................................... critical section, end.

void Socket::ShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    ShutDownSocket();
}    // ......................................................................................... critical section, end.

int Socket::Read(void *outBuffer, int bufferSize) {
    int fd;
    {
        unique_lock<mutex> critical(lock_);    // Critical section......................................................
        if (error_) {
            return 0;
        }

        if (socketDown_) {
            eof_ = true;
        }

        if (eof_) {
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

int Socket::Write(const void *data, int dataSize) {
    int fd;
    {
        unique_lock<mutex> critical(lock_);    // Critical section......................................................
        if (error_) {
            return 0;
        }

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

bool Socket::Good() const {
    unique_lock<mutex> critical(const_cast<mutex &>(lock_));    // Critical section..........................................................
    return !error_ && !eof_;
}    // ......................................................................................... critical section, end.


bool Socket::Eof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return eof_;
}    // ......................................................................................... critical section, end.

void Socket::ClearEof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    eof_ = false;
}    // ......................................................................................... critical section, end.

bool Socket::ErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return error_;
}    // ......................................................................................... critical section, end.

void Socket::SetFinalResultAcceptor(const shared_ptr<Result> &resultAcceptor) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    finalResultAcceptor_ = resultAcceptor;
}    // ......................................................................................... critical section, end.

shared_ptr<Socket> Socket::ConnectTcp(const string &host, Core::Interface *loggingObject) {
    int fd = NetworkTools::ConnectTcp(host, loggingObject);
    return (fd >= 0) ? make_shared<Socket>(fd) : nullptr;
}

shared_ptr<Socket> Socket::ConnectTcp(uint32_t ip4Address, int port, Core::Interface *loggingObject) {
    int fd = NetworkTools::ConnectTcp(ip4Address, port, loggingObject);
    return (fd >= 0) ? make_shared<Socket>(fd) : nullptr;
}

// Expects the lock to be held.
void Socket::ShutDownSocket() {
    if (fd_ != -1) {
        if (!socketDown_) {
            if (!shutdown(fd_, SHUT_RDWR)) {
                error_ = true;
            }
            socketDown_ = true;
            Log::Print(Log::Level::Debug, this, [&]{ return "socket " + to_string(fd_) + " shut down"; });
        }
    }
}

}    // Namesapce IO.
}    // Namespace K.
