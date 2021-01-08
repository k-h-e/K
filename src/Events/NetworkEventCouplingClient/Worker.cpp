#include "Worker.h"

#include "SharedState.h"
#include <K/Core/CompletionActionAdapter.h>
#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/NetworkEventCoupling.h>

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using K::Core::ThreadPool;
using K::Core::CompletionHandlerInterface;
using K::Core::CompletionActionAdapter;
using K::Core::Log;
using K::IO::TcpConnection;
using K::IO::ConnectionIO;
using K::Events::NetworkEventCoupling;

namespace K {
namespace Events {

NetworkEventCouplingClient::Worker::Worker(
    const shared_ptr<EventLoopHub> &hub, const shared_ptr<ActionInterface> &onConnectedAction,
    const shared_ptr<ActionInterface> &onFailedToConnectAction, const shared_ptr<ActionInterface> &onDisconnectedAction,
    const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<ThreadPool> &threadPool,
    shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState),
          hub_(hub),
          onConnectedAction_(onConnectedAction),
          onFailedToConnectAction_(onFailedToConnectAction),
          onDisconnectedAction_(onDisconnectedAction),
          connectionIO_(connectionIO),
          threadPool_(threadPool),
          hostIp4Address_(0u),
          hostPort_(0) {
    // Nop.
}

void NetworkEventCouplingClient::Worker::SetHost(uint32_t ip4Address, int port) {
    hostIp4Address_ = ip4Address;
    hostPort_       = port;
}

void NetworkEventCouplingClient::Worker::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    shared_ptr<TcpConnection> tcpConnection = make_shared<TcpConnection>(hostIp4Address_, hostPort_, connectionIO_);
    if (!tcpConnection->ErrorState()) {
        if (onConnectedAction_) {
            onConnectedAction_->ExecuteAction();
        }
        shared_ptr<CompletionHandlerInterface> handler;
        if (onDisconnectedAction_) {
            handler = make_shared<CompletionActionAdapter>(onDisconnectedAction_);
        }
        auto coupling = make_shared<NetworkEventCoupling>(tcpConnection, hub_, handler, 0, threadPool_);
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

