#include "Worker.h"

#include "SharedState.h"
#include <K/Core/CompletionActionAdapter.h>
#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/NetworkEventCoupling.h>

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using K::Core::CompletionActionAdapter;
using K::Core::CompletionHandlerInterface;
using K::Core::Log;
using K::Core::ThreadPool;
using K::Core::Timers;
using K::IO::TcpConnection;
using K::IO::ConnectionIO;
using K::Events::NetworkEventCoupling;

namespace K {
namespace Events {

NetworkEventCouplingClient::Worker::Worker(
    const shared_ptr<EventLoopHub> &hub, const shared_ptr<ActionInterface> &onConnectedAction,
    const shared_ptr<ActionInterface> &onFailedToConnectAction, const shared_ptr<ActionInterface> &onDisconnectedAction,
    const shared_ptr<SharedState> &sharedState, const shared_ptr<ConnectionIO> &connectionIO,
    const shared_ptr<ThreadPool> &threadPool, const shared_ptr<Timers> &timers)
        : sharedState_(sharedState),
          hub_(hub),
          onConnectedAction_(onConnectedAction),
          onFailedToConnectAction_(onFailedToConnectAction),
          onDisconnectedAction_(onDisconnectedAction),
          connectionIO_(connectionIO),
          threadPool_(threadPool),
          timers_(timers),
          hostPort_(0) {
    // Nop.
}

void NetworkEventCouplingClient::Worker::SetHost(const string &host, int port) {
    host_     = host;
    hostPort_ = port;
}

void NetworkEventCouplingClient::Worker::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    shared_ptr<TcpConnection> tcpConnection = make_shared<TcpConnection>(host_, hostPort_, connectionIO_);
    if (!tcpConnection->ErrorState()) {
        if (onConnectedAction_) {
            onConnectedAction_->ExecuteAction();
        }
        shared_ptr<CompletionHandlerInterface> handler;
        if (onDisconnectedAction_) {
            handler = make_shared<CompletionActionAdapter>(onDisconnectedAction_);
        }
        auto coupling = make_shared<NetworkEventCoupling>(tcpConnection, hub_, handler, 0, threadPool_, timers_);
        sharedState_->OnCouplingCreated(coupling);
    }
    else {
        if (onFailedToConnectAction_) {
            onFailedToConnectAction_->ExecuteAction();
        }
    }

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace Events.
}    // Namespace K.

