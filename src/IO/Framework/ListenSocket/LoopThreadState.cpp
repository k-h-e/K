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
#include <K/IO/Framework/TcpConnection.h>
#include "SynchronizedState.h"

using std::move;
using std::shared_ptr;
using std::unique_ptr;
using K::Core::Log;
using K::Core::Framework::RunLoop;
using K::IO::ConnectionIO;

namespace K {
namespace IO {
namespace Framework {

ListenSocket::LoopThreadState::LoopThreadState(
    const shared_ptr<RunLoop> &aRunLoop, const shared_ptr<SynchronizedState> &aSynchronizedState,
    const shared_ptr<IO::ListenSocket> &aListenSocket, const std::shared_ptr<ConnectionIO> &aConnectionIO)
        : synchronizedState(aSynchronizedState),
          listenSocket(aListenSocket),
          connectionIO(aConnectionIO),
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

    while (!acceptedConnections.empty()) {
        unique_ptr<TcpConnection> connection;
        connection.swap(acceptedConnections.front());
        acceptedConnections.pop_front();
        if (error) {
            Log::Print(Log::Level::Warning, this, [&]{ return "in error state, closing accepted connection"; });
        } else if (!handler) {
            Log::Print(Log::Level::Warning, this, [&]{ return "no handler registered, closing accepted connection"; });
        } else {
            handler->OnListenSocketAcceptedConnection(handlerAssociatedId, move(connection));
        }
    }
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
