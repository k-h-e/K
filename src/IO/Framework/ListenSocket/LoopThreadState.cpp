////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoopThreadState.h"

#include "SynchronizedState.h"

using std::shared_ptr;
using K::Core::Framework::RunLoop;

namespace K {
namespace IO {
namespace Framework {

ListenSocket::LoopThreadState::LoopThreadState(
    const shared_ptr<RunLoop> &aRunLoop, const shared_ptr<SynchronizedState> &aSynchronizedState,
    const shared_ptr<IO::ListenSocket> &aListenSocket)
        : synchronizedState(aSynchronizedState),
          listenSocket(aListenSocket),
          runLoop(aRunLoop),
          runLoopClientId(0),
          handler(nullptr),
          handlerAssociatedId(0),
          error(false),
          newHandlerRegistered(false),
          activationRequested(false) {
    // Nop.
}

void ListenSocket::LoopThreadState::RequestActivation() {
    if (!activationRequested) {
        runLoop->RequestActivation(runLoopClientId);
        activationRequested = true;
    }
}

void ListenSocket::LoopThreadState::Activate() {
    bool signalErrorState = false;

    bool wasError = error;
    synchronizedState->Sync(this);

    if (error && !wasError) {
        signalErrorState = true;
    }

    if (newHandlerRegistered) {
        if (error) {
            signalErrorState = true;
        }
        newHandlerRegistered = false;
    }

    activationRequested = false;

    // Handler activation...

    if (signalErrorState) {
        if (handler) {
            handler->OnListenSocketErrorState(handlerAssociatedId);
        }
    }
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
