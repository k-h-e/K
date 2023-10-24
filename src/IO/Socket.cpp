/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/Socket.h>

#include <unistd.h>
#include <sys/socket.h>

#include <vector>

#include <K/Core/StringTools.h>
#include <K/Core/ResultAcceptor.h>
#include <K/Core/Log.h>
#include <K/IO/IOTools.h>
#include <K/IO/NetworkTools.h>

using std::make_shared;
using std::mutex;
using std::optional;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::unique_lock;
using std::vector;

using K::Core::Log;
using K::Core::ResultAcceptor;
using K::Core::StreamInterface;
using K::Core::StringTools;
using K::IO::IOTools;
using K::IO::NetworkTools;

namespace K {
namespace IO {

Socket::Socket(int fd)
        : socketDown_(false) {
    Log::Print(Log::Level::Debug, this, [=]{ return "fd=" + to_string(fd); });
    if (fd < 0) {
        fd          = -1;
        socketDown_ = true;
        error_      = Error::IO;
    }

    fd_ = fd;
}

Socket::~Socket() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    ShutDownSocket();
    if (fd_ != -1) {
        if (!IOTools::CloseFileDescriptor(fd_, this)) {
            error_ = Error::IO;
        }
    }

    if (closeResultAcceptor_) {
        if (error_) {
            closeResultAcceptor_->OnFailure();
        } else {
            closeResultAcceptor_->OnSuccess();
        }
    }

    if (error_) {
        Log::Print(Log::Level::Error, this, [&]{ return "error state after closing, fd=" + to_string(fd_); });
    } else {
        Log::Print(Log::Level::Debug, this, [&]{ return "closed, fd=" + to_string(fd_); });
    }
}    // ......................................................................................... critical section, end.

void Socket::ShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    ShutDownSocket();
}    // ......................................................................................... critical section, end.

int Socket::ReadBlocking(void *buffer, int bufferSize) {
    int fd;
    {
        unique_lock<mutex> critical(lock_);    // Critical section......................................................
        if (error_) {
            return 0;
        }

        if (socketDown_) {
            error_ = Error::Eof;
        }

        if (error_) {
            return 0;
        }

        fd = fd_;
    }    // ..................................................................................... critical section, end.

    while (true) {
        int num = read(fd, buffer, bufferSize);
        if (num == 0) {
            unique_lock<mutex> critical(lock_);    // Critical section..................................................
            ShutDownSocket();
            if (!error_) {
                error_ = Error::Eof;
            }
            return 0;
        }    // ................................................................................. critical section, end.
        else if (num == -1) {
            if (errno != EINTR) {
                unique_lock<mutex> critical(lock_);    // Critical section..............................................
                ShutDownSocket();
                if (!error_) {
                    error_ = Error::IO;
                }
                return 0;
            }    // ............................................................................. critical section, end.
        }
        else {
            return num;
        }
    }
}

int Socket::WriteBlocking(const void *data, int dataSize) {
    int fd;
    {
        unique_lock<mutex> critical(lock_);    // Critical section......................................................
        if (error_) {
            return 0;
        }

        if (socketDown_) {
            error_ = Error::IO;
            return 0;
        }

        fd = fd_;
    }    // ..................................................................................... critical section, end.

    while (true) {
        int num = write(fd, data, dataSize);
        if (num == -1) {
            if (errno != EINTR) {
                unique_lock<mutex> critical(lock_);    // Critical section..............................................
                ShutDownSocket();
                if (!error_) {
                    error_ = Error::IO;
                }
                return 0;
            }    // ............................................................................. critical section, end.
        } else {
            if (num > 0) {
                return num;
            }
        }
    }
}

bool Socket::ErrorState() const {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return (error_.has_value());
}    // ......................................................................................... critical section, end.

optional<StreamInterface::Error> Socket::StreamError() const {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return error_;
}    // ......................................................................................... critical section, end.


void Socket::SetCloseResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    closeResultAcceptor_ = resultAcceptor;
}    // ......................................................................................... critical section, end.

shared_ptr<Socket> Socket::ConnectTcp(const string &host, Core::Interface *loggingObject) {
    optional<int> fd = NetworkTools::ConnectTcp(host, loggingObject);
    return fd ? make_shared<Socket>(*fd) : nullptr;
}

shared_ptr<Socket> Socket::ConnectTcp(uint32_t ip4Address, int port, Core::Interface *loggingObject) {
    optional<int> fd = NetworkTools::ConnectTcp(ip4Address, port, loggingObject);
    return fd ? make_shared<Socket>(*fd) : nullptr;
}

// Expects the lock to be held.
void Socket::ShutDownSocket() {
    if (fd_ != -1) {
        if (!socketDown_) {
            if (!shutdown(fd_, SHUT_RDWR)) {
                if (!error_) {
                    error_ = Error::IO;
                }
            }
            socketDown_ = true;
            Log::Print(Log::Level::Debug, this, [&]{ return "socket " + to_string(fd_) + " shut down"; });
        }
    }
}

}    // Namesapce IO.
}    // Namespace K.
