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

Connection::LoopThreadState::LoopThreadState(
    const shared_ptr<RunLoop> &aRunLoop, const shared_ptr<SynchronizedState> &aSynchronizedState,
    int aBufferSizeConstraint, const shared_ptr<ConnectionIO> &aConnectionIO)
        : synchronizedState(aSynchronizedState),
          connectionIO(aConnectionIO),
          runLoop(aRunLoop),
          runLoopClientId(0),
          handler(nullptr),
          handlerAssociatedId(0),
          readFailed(false),
          writeFailed(false),
          error(false),
          eof(false),
          clientReadPaused(false),
          clientWritePaused(false),
          newHandlerRegistered(false),
          unpauseIORead(false),
          unpauseIOWrite(false),
          bufferSizeConstraint(aBufferSizeConstraint),
          activationRequested(false) {
    // Nop.
}

void Connection::LoopThreadState::RequestActivation() {
    if (!activationRequested) {
        runLoop->RequestActivation(runLoopClientId);
        activationRequested = true;
    }
}

void Connection::LoopThreadState::Activate() {
    bool signalReadyRead  = false;
    bool signalReadyWrite = false;

    synchronizedState->Sync(this);

    if (clientReadPaused && !readBuffer.Empty()) {
        signalReadyRead = true;
        clientReadPaused = false;
    }
    if (clientWritePaused && (writeBuffer.Size() < bufferSizeConstraint)) {
        signalReadyWrite = true;
        clientWritePaused = false;
    }

    if (unpauseIORead) {
        connectionIO->SetClientCanRead(synchronizedState.get());
        unpauseIORead = false;
    }
    if (unpauseIOWrite) {
        connectionIO->SetClientCanWrite(synchronizedState.get());
        unpauseIOWrite = false;
    }

    if (newHandlerRegistered) {
        signalReadyRead  = true;
        signalReadyWrite = true;
        newHandlerRegistered = false;
    }

    activationRequested = false;

    // Handler activation...

    if (signalReadyRead) {
        if (handler) {
            handler->OnStreamReadyRead(handlerAssociatedId);
        }
    }
    if (signalReadyWrite) {
        if (handler) {
            handler->OnStreamReadyWrite(handlerAssociatedId);
        }
    }
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
