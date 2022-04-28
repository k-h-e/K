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

using std::mutex;
using std::optional;
using std::shared_ptr;
using std::unique_lock;
using K::Core::Framework::RunLoop;
using K::IO::IOTools;

namespace K {
namespace IO {
namespace Framework {

TcpConnector::SynchronizedState::SynchronizedState(const shared_ptr<RunLoop> &runLoop, int runLoopClientId)
        : runLoop_{runLoop},
          runLoopClientId_{runLoopClientId},
          finished_{false} {
}

TcpConnector::SynchronizedState::~SynchronizedState() {
    if (fd_) {
        IOTools::CloseFileDescriptor(*fd_, this);
    }
}

void TcpConnector::SynchronizedState::Sync(optional<int> *fd, bool *finished) {
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    *fd       = fd_;
    fd_.reset();
    *finished = finished_;
}    // ......................................................................................... critical section, end.

void TcpConnector::SynchronizedState::OnTcpConnectionEstablished(int id, int fd) {
    (void)id;
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    fd_       = fd;
    finished_ = true;
    runLoop_->RequestActivation(runLoopClientId_, true);
}    // ......................................................................................... critical section, end.

void TcpConnector::SynchronizedState::OnFailedToEstablishTcpConnection(int id) {
    (void)id;
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    finished_ = true;
    runLoop_->RequestActivation(runLoopClientId_, true);
}    // ......................................................................................... critical section, end.

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
