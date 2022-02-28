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

#include "LoopThreadState.h"

using std::mutex;
using std::shared_ptr;
using std::unique_lock;
using K::Core::Framework::RunLoop;

namespace K {
namespace IO {
namespace Framework {

ListenSocket::SynchronizedState::SynchronizedState(const shared_ptr<RunLoop> &runLoop)
        : runLoop_(runLoop),
          error_(false),
          syncRequested_(false) {
    // Nop.
}

void ListenSocket::SynchronizedState::SetRunLoopClientId(int id) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    runLoopClientId_ = id;
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::Sync(LoopThreadState *loopThreadState) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................

    if (error_) {
        loopThreadState->error = true;
    }

    syncRequested_ = false;

}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::OnListenSocketAcceptedConnection(const shared_ptr<TcpConnection> &connection) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    (void)connection;
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
