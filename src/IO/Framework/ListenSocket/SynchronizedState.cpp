////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SynchronizedState.h"

#include <K/IO/IOTools.h>
#include <K/IO/Framework/TcpConnection.h>
#include "LoopThreadState.h"

using std::make_shared;
using std::move;
using std::mutex;
using std::shared_ptr;
using std::unique_lock;
using K::Core::Framework::RunLoop;
using K::IO::IOTools;

namespace K {
namespace IO {
namespace Framework {

ListenSocket::SynchronizedState::SynchronizedState(const shared_ptr<RunLoop> &runLoop)
        : runLoop_(runLoop),
          error_(false),
          syncRequested_(false) {
    // Nop.
}

ListenSocket::SynchronizedState::~SynchronizedState() {
    for (int fd : acceptedConnections_) {
        (void)IOTools::CloseFileDescriptor(fd, this);
    }
}

void ListenSocket::SynchronizedState::SetRunLoopClientId(int id) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    runLoopClientId_ = id;
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::Sync(LoopThreadState *loopThreadState) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    while (!acceptedConnections_.empty()) {
        auto connection = make_unique<TcpConnection>(acceptedConnections_.front(), runLoop_,
                                                     loopThreadState->connectionIO);
        acceptedConnections_.pop_front();
        loopThreadState->acceptedConnections.push_back(move(connection));
    }
    if (error_) {
        loopThreadState->error = true;
    }
    syncRequested_ = false;
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::OnListenSocketAcceptedConnection(
        const shared_ptr<IO::TcpConnection> &connection) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    (void)connection;
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::OnListenSocketAcceptedConnection(int fd) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    acceptedConnections_.push_back(fd);
    RequestSync(critical);
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::OnListenSocketErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    if (!error_) {
        error_ = true;
        RequestSync(critical);
    }
}    // ......................................................................................... critical section, end.

// Expects lock to be held.
void ListenSocket::SynchronizedState::RequestSync(unique_lock<mutex> &critical) {
    (void)critical;
    if (!syncRequested_) {
        if (runLoopClientId_) {
            runLoop_->RequestActivation(*runLoopClientId_);
        }
        syncRequested_ = true;
    }
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
