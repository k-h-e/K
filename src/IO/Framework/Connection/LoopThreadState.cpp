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

#include <K/Core/Log.h>
#include "SynchronizedState.h"

using std::shared_ptr;
using std::to_string;
using K::Core::Log;
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
          error(false),
          eof(false),
          clientReadPaused(false),
          clientWritePaused(false),
          handlerNeedsReadyRead(false),
          handlerNeedsReadyWrite(false),
          unpauseIORead(false),
          unpauseIOWrite(false),
          readIsNext(false),
          bufferSizeConstraint(aBufferSizeConstraint),
          activationRequested(false),
          requestedActivationIsDeep(false) {
    // Nop.
}

void Connection::LoopThreadState::RequestActivation(bool deepActivation) {
    if (!activationRequested || (deepActivation && !requestedActivationIsDeep)) {
        runLoop->RequestActivation(runLoopClientId, deepActivation);
        activationRequested = true;
        if (deepActivation) {
            requestedActivationIsDeep = true;
        }
    }
}

void Connection::LoopThreadState::Activate(bool deepActivation) {
    activationRequested       = false;
    requestedActivationIsDeep = false;

    Log::Print(Log::Level::DebugDebug, this, [&]{ return "Activate(), deep=" + to_string(deepActivation); });

    if (deepActivation && !error) {
        synchronizedState->Sync(this);
        if (unpauseIORead) {
            connectionIO->SetClientCanRead(synchronizedState.get());
            unpauseIORead = false;
        }
        if (unpauseIOWrite) {
            connectionIO->SetClientCanWrite(synchronizedState.get());
            unpauseIOWrite = false;
        }
    }

    bool signalReadyRead  = false;
    bool signalReadyWrite = false;

    if ((clientReadPaused && !readBuffer.Empty()) || handlerNeedsReadyRead) {
        signalReadyRead = true;
    }
    if ((clientWritePaused && (writeBuffer.Size() < bufferSizeConstraint)) || handlerNeedsReadyWrite) {
        signalReadyWrite = true;
    }

    if (signalReadyRead && signalReadyWrite) {
        RequestActivation(false);
        if (readIsNext) {
            signalReadyWrite = false;
        } else {
            signalReadyRead = false;
        }
    }

    if (signalReadyRead) {
        clientReadPaused      = false;
        handlerNeedsReadyRead = false;
        readIsNext            = false;
        Log::Print(Log::Level::DebugDebug, this, [&]{ return "ready read"; });
        if (handler) {
            handler->OnStreamReadyRead(handlerAssociatedId);
        }
    } else if (signalReadyWrite) {
        clientWritePaused      = false;
        handlerNeedsReadyWrite = false;
        readIsNext             = true;
        Log::Print(Log::Level::DebugDebug, this, [&]{ return "ready write"; });
        if (handler) {
            handler->OnStreamReadyWrite(handlerAssociatedId);
        }
    }
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
