/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "LoopThreadState.h"

#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>
#include "SynchronizedState.h"

using std::shared_ptr;
using std::unique_ptr;
using K::Core::Log;
using K::Core::RunLoop;
using K::IO::ConnectionIO;

namespace K {
namespace IO {

ListenSocket::LoopThreadState::LoopThreadState(
    const shared_ptr<RunLoop> &aRunLoop, const shared_ptr<SynchronizedState> &aSynchronizedState,
    const shared_ptr<Deprecated::ListenSocket> &aListenSocket, const std::shared_ptr<ConnectionIO> &aConnectionIO)
        : synchronizedState(aSynchronizedState),
          listenSocket(aListenSocket),
          connectionIO(aConnectionIO),
          runLoop(aRunLoop),
          runLoopClientId(0),
          handler(nullptr),
          error(false),
          newHandlerRegistered(false),
          activationRequested(false),
          requestedActivationIsDeep(false) {
    // Nop.
}

void ListenSocket::LoopThreadState::RequestActivation(bool deepActivation) {
    if (!activationRequested || (deepActivation && !requestedActivationIsDeep)) {
        runLoop->RequestActivation(runLoopClientId, deepActivation);
        activationRequested = true;
        if (deepActivation) {
            requestedActivationIsDeep = true;
        }
    }
}

void ListenSocket::LoopThreadState::Activate(bool deepActivation) {
    activationRequested       = false;
    requestedActivationIsDeep = false;

    bool signalErrorState = false;

    if (deepActivation && !error) {
        synchronizedState->Sync(this);
        if (error) {
            signalErrorState = true;
        }
    }

    if (newHandlerRegistered) {
        if (error) {
            signalErrorState = true;
        }
        newHandlerRegistered = false;
    }

    if (signalErrorState) {
        if (handler) {
            handler->OnListenSocketErrorState();
        }
    } else if (!error && !acceptedConnections.empty()) {
        unique_ptr<TcpConnection> connection;
        connection.swap(acceptedConnections.front());
        acceptedConnections.pop_front();
        if (!acceptedConnections.empty()) {
            RequestActivation(false);
        }

        if (handler) {
            handler->OnListenSocketAcceptedConnection(std::move(connection));
        } else {
            Log::Print(Log::Level::Warning, this, [&]{ return "no handler registered, closing accepted connection"; });
        }
    }
}

}    // Namespace IO.
}    // Namespace K.
