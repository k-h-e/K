/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "SynchronizedState.h"

#include <K/IO/IOTools.h>
#include <K/IO/TcpConnection.h>
#include "LoopThreadState.h"

using std::make_shared;
using std::mutex;
using std::shared_ptr;
using std::unique_lock;
using K::Core::RunLoop;
using K::IO::IOTools;

namespace K {
namespace IO {

ListenSocket::SynchronizedState::SynchronizedState(const shared_ptr<RunLoop> &runLoop)
        : runLoop_(runLoop),
          error_(false),
          syncRequested_(false) {
    // Nop.
}

ListenSocket::SynchronizedState::~SynchronizedState() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    DropAcceptedConnections(critical);
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::SetRunLoopClientId(int id) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    runLoopClientId_ = id;
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::Sync(LoopThreadState *loopThreadState) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    syncRequested_ = false;
    if (error_) {
        if (!loopThreadState->error) {
            loopThreadState->acceptedConnections.clear();
            loopThreadState->error = true;
        }
    } else {
        while (!acceptedConnections_.empty()) {
            auto connection = make_unique<TcpConnection>(acceptedConnections_.front(), runLoop_,
                                                         loopThreadState->connectionIO, loopThreadState->ioBuffers);
            acceptedConnections_.pop_front();
            loopThreadState->acceptedConnections.push_back(std::move(connection));
        }
    }
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::OnListenSocketAcceptedConnection(
        const shared_ptr<Deprecated::TcpConnection> &connection) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    (void)connection;
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::OnListenSocketAcceptedConnection(int fd) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    if (!error_) {
        acceptedConnections_.push_back(fd);
        RequestSync(critical);
    } else {
        (void)IOTools::CloseFileDescriptor(fd, this);
    }
}    // ......................................................................................... critical section, end.

void ListenSocket::SynchronizedState::OnListenSocketErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    if (!error_) {
        DropAcceptedConnections(critical);
        error_ = true;
        RequestSync(critical);
    }
}    // ......................................................................................... critical section, end.

// Expects lock to be held.
void ListenSocket::SynchronizedState::RequestSync(unique_lock<mutex> &critical) {
    (void)critical;
    if (!syncRequested_) {
        if (runLoopClientId_) {
            runLoop_->RequestActivation(*runLoopClientId_, true);
        }
        syncRequested_ = true;
    }
}

// Expects lock to be held.
void ListenSocket::SynchronizedState::DropAcceptedConnections(unique_lock<mutex> &critical) {
    (void)critical;
    for (int fd : acceptedConnections_) {
        (void)IOTools::CloseFileDescriptor(fd, this);
    }
    acceptedConnections_.clear();
}

}    // Namespace IO.
}    // Namespace K.
