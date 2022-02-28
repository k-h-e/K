#include "SharedState.h"

#include <K/Core/Log.h>
#include <K/Core/ThreadPool.h>
#include <K/IO/IOTools.h>
#include <K/IO/NetworkTools.h>
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
using K::IO::TcpConnection;

namespace K {
namespace IO {

ListenSocket::SharedState::SharedState(int port, const shared_ptr<ConnectionIO> &connectionIO,
                                       const shared_ptr<ThreadPool> &threadPool)
        : port_(port),
          handler_(nullptr),
          handlerUpdatedInitially_(false),
          acceptorThreadRunning_(false),
          error_(false),
          shuttingDown_(false),
          connectionIO_(connectionIO),
          threadPool_(threadPool) {
    acceptor_ = make_unique<Acceptor>(port, this);

    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    UpdateAcceptor(critical);
}    // ......................................................................................... critical section, end.

ListenSocket::SharedState::~SharedState() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    shuttingDown_ = true;
    if (acceptorThreadRunning_) {
        (void)NetworkTools::ConnectTcp(0x7f000001u, port_, this);    // -> local host.
        while (acceptorThreadRunning_) {
            stateChanged_.wait(critical);
        }
    }
}    // ......................................................................................... critical section, end.

void ListenSocket::SharedState::Register(HandlerInterface *handler) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    handler_ = handler;
    if (handler_) {
        handlerUpdatedInitially_ = false;
    }
    UpdateAcceptor(critical);
}    // ......................................................................................... critical section, end.

bool ListenSocket::SharedState::ErrorState() const {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    return error_;
}    // ......................................................................................... critical section, end.

void ListenSocket::SharedState::OnConnectionAccepted(int fd) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    EnsureHandlerUpdatedInitially(critical);
    if (!error_) {
        if (handler_) {
            auto connection = make_shared<TcpConnection>(fd, connectionIO_);
            handler_->OnNetworkConnectionAccepted(connection);
        } else {
            Log::Print(Log::Level::Warning, this, [&]{
                return "no handler registered, immediately closing accepted connection " + to_string(fd);
            });
            IOTools::CloseFileDescriptor(fd, this);
        }
    } else {
        Log::Print(Log::Level::Warning, this, [&]{
            return "in error state, immediately closing accepted connection " + to_string(fd);
        });
        IOTools::CloseFileDescriptor(fd, this);
    }
}    // ......................................................................................... critical section, end.

void ListenSocket::SharedState::ReportError() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    EnsureHandlerUpdatedInitially(critical);
    if (!error_) {
        if (handler_) {
            handler_->OnListenSocketErrorState();
        }
        error_ = true;
    }
    UpdateAcceptor(critical);
}    // ......................................................................................... critical section, end.

void ListenSocket::SharedState::OnCompletion(int completionId) {
    (void)completionId;
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    EnsureHandlerUpdatedInitially(critical);
    acceptorThreadRunning_ = false;
    UpdateAcceptor(critical);
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

// Expects lock to be held.
void ListenSocket::SharedState::UpdateAcceptor(unique_lock<mutex> &critical) {
    (void)critical;
    if (!acceptorThreadRunning_ && !shuttingDown_ && (!error_ || !handlerUpdatedInitially_)) {
        if (error_) {
            acceptor_->SetErrorState();
        }
        threadPool_->Run(acceptor_.get(), this, 0);
        acceptorThreadRunning_ = true;
    }
}

// Expects lock to be held.
void ListenSocket::SharedState::EnsureHandlerUpdatedInitially(unique_lock<mutex> &critical) {
    (void)critical;
    if (!handlerUpdatedInitially_) {
        if (handler_) {
            if (error_) {
                handler_->OnListenSocketErrorState();
            }
        }

        handlerUpdatedInitially_ = true;
    }
}

}    // Namesapce IO.
}    // Namespace K.
