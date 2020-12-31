#include "Worker.h"

#include <K/Core/Log.h>
#include <K/IO/ListenSocket.h>
#include <K/Events/NetworkEventCoupling.h>
#include "SharedState.h"

using std::shared_ptr;
using std::unique_ptr;
using K::Core::ThreadPool;
using K::Core::Log;
using K::IO::ListenSocket;
using K::IO::SocketStream;
using K::Events::NetworkEventCoupling;

namespace K {
namespace Events {

NetworkEventCouplingServer::Worker::Worker(
    const shared_ptr<ListenSocket> &listenSocket, const shared_ptr<EventLoopHub> &hub,
    const shared_ptr<ThreadPool> &threadPool, const shared_ptr<SharedState> &sharedState)
        : sharedState_(sharedState),
          listenSocket_(listenSocket),
          hub_(hub),
          threadPool_(threadPool) {
    // Nop.
}

void NetworkEventCouplingServer::Worker::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    while (!listenSocket_->ErrorState() && !sharedState_->ShutDownRequested()) {
        Log::Print(Log::Level::Debug, this, []{ return "waiting for client to connect..."; });
        shared_ptr<SocketStream> stream = listenSocket_->Accept();
        if (stream && !sharedState_->ShutDownRequested()) {
            sharedState_->PrepareForCoupling();
            auto coupling = unique_ptr<NetworkEventCoupling>(
                new NetworkEventCoupling(stream, hub_, sharedState_, couplingCompletionId, threadPool_));
            Log::Print(Log::Level::Debug, this, []{ return "waiting for coupling to finish..."; });
            sharedState_->WaitForCouplingFinished();
        }
    }

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace Events.
}    // Namespace K.

