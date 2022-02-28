////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/IO/Framework/ListenSocket.h>

#include "LoopThreadState.h"
#include "SynchronizedState.h"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using K::Core::ThreadPool;
using K::Core::Framework::RunLoop;
using K::IO::ConnectionIO;

namespace K {
namespace IO {
namespace Framework {

ListenSocket::ListenSocket(int port, const shared_ptr<RunLoop> &runLoop,
                           const shared_ptr<ConnectionIO> &connectionIO, const std::shared_ptr<ThreadPool> &threadPool)
        : loopThreadState_(make_unique<LoopThreadState>(
              runLoop, make_shared<SynchronizedState>(runLoop),
              make_shared<IO::ListenSocket>(port, connectionIO, threadPool))) {
    loopThreadState_->runLoopClientId = loopThreadState_->runLoop->AddClient(loopThreadState_.get());
    loopThreadState_->synchronizedState->SetRunLoopClientId(loopThreadState_->runLoopClientId);

    loopThreadState_->listenSocket->Register(loopThreadState_->synchronizedState.get());
}

ListenSocket::~ListenSocket() {
    loopThreadState_->listenSocket->Register(nullptr);
    loopThreadState_->runLoop->RemoveClient(loopThreadState_->runLoopClientId);
}

void ListenSocket::Register(HandlerInterface *handler, int id) {
    if (handler) {
        loopThreadState_->handler              = handler;
        loopThreadState_->handlerAssociatedId  = id;
        loopThreadState_->newHandlerRegistered = true;
        loopThreadState_->RequestActivation();
    } else {
        loopThreadState_->handler             = nullptr;
        loopThreadState_->handlerAssociatedId = 0;
    }
}

bool ListenSocket::ErrorState() const {
    return loopThreadState_->error;
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
