/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/ListenSocket.h>

#include <K/IO/TcpConnection.h>
#include "LoopThreadState.h"
#include "SynchronizedState.h"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using K::Core::RunLoop;
using K::Core::ThreadPool;
using K::IO::ConnectionIO;

namespace K {
namespace IO {

ListenSocket::ListenSocket(int port, const shared_ptr<RunLoop> &runLoop,
                           const shared_ptr<ConnectionIO> &connectionIO, const std::shared_ptr<ThreadPool> &threadPool)
        : loopThreadState_(make_unique<LoopThreadState>(
              runLoop, make_shared<SynchronizedState>(runLoop),
              make_shared<Deprecated::ListenSocket>(port, connectionIO, threadPool), connectionIO)) {
    loopThreadState_->runLoopClientId = loopThreadState_->runLoop->AddClient(loopThreadState_.get());
    loopThreadState_->synchronizedState->SetRunLoopClientId(loopThreadState_->runLoopClientId);

    loopThreadState_->listenSocket->Register(loopThreadState_->synchronizedState.get(), true);
}

ListenSocket::~ListenSocket() {
    loopThreadState_->listenSocket->Register(nullptr, false);
    loopThreadState_->runLoop->RemoveClient(loopThreadState_->runLoopClientId);
}

void ListenSocket::Register(HandlerInterface *handler) {
    if (handler) {
        loopThreadState_->handler              = handler;
        loopThreadState_->newHandlerRegistered = true;
        loopThreadState_->RequestActivation(false);
    } else {
        loopThreadState_->handler             = nullptr;
    }
}

bool ListenSocket::ErrorState() const {
    return loopThreadState_->error;
}

}    // Namespace IO.
}    // Namespace K.
