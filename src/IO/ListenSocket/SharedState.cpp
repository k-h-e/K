#include "SharedState.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <K/Core/Log.h>
#include <K/Core/ThreadPool.h>
#include <K/IO/IOTools.h>
#include <K/IO/NetworkTools.h>
#include <K/IO/Socket.h>
#include <K/IO/TcpConnection.h>
#include "Acceptor.h"

using std::make_shared;
using std::make_unique;
using std::mutex;
using std::shared_ptr;
using std::to_string;
using std::unique_lock;
using K::Core::Log;
using K::Core::ThreadPool;
using K::IO::IOTools;
using K::IO::NetworkTools;
using K::IO::Socket;
using K::IO::TcpConnection;

namespace K {
namespace IO {

ListenSocket::SharedState::SharedState(int port, const shared_ptr<ConnectionIO> &connectionIO,
                                       const shared_ptr<ThreadPool> &threadPool)
        : port_(port),
          numAcceptRequests_(0),
          acceptedConnection_(-1),
          acceptorThreadRunning_(false),
          connectionIO_(connectionIO),
          threadPool_(threadPool),
          error_(true) {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ != -1) {
        struct sockaddr_in address = {};
        address.sin_family      = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port        = htons(static_cast<uint16_t>(port));
        if (!bind(fd_, (struct sockaddr *)&address, sizeof(address))) {
            if (!listen(fd_, 4)) {
                acceptor_ = make_unique<Acceptor>(fd_, this);
                error_    = false;
                Log::Print(Log::Level::Debug, this, [=]{ return "socket " + to_string(fd_) + " listening on port "
                    + to_string(port); });
            }
        }
    }

    if (error_) {
        Log::Print(Log::Level::Error, this, [=]{ return "failed to listen on port " + to_string(port); });
    }
}

ListenSocket::SharedState::~SharedState() {
    ShutDown();
    if (fd_ != -1) {
        IOTools::CloseFileDescriptor(fd_, this);
    }
}

shared_ptr<Socket> ListenSocket::SharedState::Accept() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    int fd = DoAccept(critical);
    return (fd != -1) ? make_shared<Socket>(fd) : nullptr;
}    // ......................................................................................... critical section, end.

shared_ptr<TcpConnection> ListenSocket::SharedState::AcceptConnection() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    int fd = DoAccept(critical);
    return (fd != -1) ? make_shared<TcpConnection>(fd, connectionIO_) : nullptr;
}    // ......................................................................................... critical section, end.

void ListenSocket::SharedState::ShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    error_ = true;
    if (acceptorThreadRunning_) {
        (void)NetworkTools::ConnectTcp(0x7f000001u, port_, this);    // -> local host.
        while (acceptorThreadRunning_) {
            stateChanged_.wait(critical);
        }
    }
}    // ......................................................................................... critical section, end.

void ListenSocket::SharedState::OnConnectionAccepted(int fd) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    if (!error_) {
        acceptedConnection_ = fd;
        stateChanged_.notify_all();
    }
}    // ......................................................................................... critical section, end.

bool ListenSocket::SharedState::ErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    return error_;
}    // ......................................................................................... critical section, end.

void ListenSocket::SharedState::OnCompletion(int completionId) {
    (void)completionId;
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    acceptorThreadRunning_ = false;
    UpdateAcceptor(critical);
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

// Expects lock to be held.
int ListenSocket::SharedState::DoAccept(unique_lock<mutex> &critical) {
    (void)critical;

    ++numAcceptRequests_;
    UpdateAcceptor(critical);
    stateChanged_.notify_all();

    while (!error_) {
        if (acceptedConnection_ != -1) {
            int fd = acceptedConnection_;
            acceptedConnection_ = -1;
            --numAcceptRequests_;
            UpdateAcceptor(critical);
            stateChanged_.notify_all();
            return fd;
        } else {
            stateChanged_.wait(critical);
        }
    }

    return -1;
}

// Expects lock to be held.
void ListenSocket::SharedState::UpdateAcceptor(unique_lock<mutex> &critical) {
    (void)critical;
    if (!acceptorThreadRunning_ && !error_ && numAcceptRequests_ && (acceptedConnection_ == -1)) {
        threadPool_->Run(acceptor_.get(), this, 0);
        acceptorThreadRunning_ = true;
    }
}

}    // Namesapce IO.
}    // Namespace K.
