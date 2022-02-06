#include "Worker.h"

#include <K/Core/Log.h>
#include <K/IO/ListenSocket.h>
#include <K/Events/NetworkEventCoupling.h>
#include "SharedState.h"

using std::make_unique;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using K::Core::Log;
using K::Core::ThreadPool;
using K::Core::Timers;
using K::IO::ListenSocket;
using K::IO::TcpConnection;
using K::Events::NetworkEventCoupling;

namespace K {
namespace Events {

NetworkEventCouplingServer::Worker::Worker(
    const shared_ptr<ListenSocket> &listenSocket, const string &protocolVersion, const shared_ptr<EventLoopHub> &hub,
    const shared_ptr<SharedState> &sharedState, const shared_ptr<ThreadPool> &threadPool,
    const shared_ptr<Timers> &timers)
        : sharedState_(sharedState),
          hub_(hub),
          threadPool_(threadPool),
          timers_(timers),
          listenSocket_(listenSocket),
          protocolVersion_(protocolVersion) {
    // Nop.
}

void NetworkEventCouplingServer::Worker::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    while (!listenSocket_->ErrorState() && !sharedState_->ShutDownRequested()) {
        Log::Print(Log::Level::Debug, this, []{ return "waiting for client to connect..."; });
        shared_ptr<TcpConnection> tcpConnection = listenSocket_->AcceptConnection();
        if (tcpConnection && !sharedState_->ShutDownRequested()) {
            sharedState_->PrepareForCoupling();
            auto coupling = make_unique<NetworkEventCoupling>(tcpConnection, protocolVersion_, hub_, sharedState_,
                                                              couplingCompletionId, threadPool_, timers_);
            tcpConnection.reset();
            Log::Print(Log::Level::Debug, this, []{ return "network event coupling installed"; });
            sharedState_->WaitForCouplingFinished();
        }
    }

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace Events.
}    // Namespace K.

