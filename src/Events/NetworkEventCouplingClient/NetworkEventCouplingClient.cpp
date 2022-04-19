#include <K/Events/NetworkEventCouplingClient.h>

#include <K/Core/ThreadPool.h>
#include "SharedState.h"
#include "Worker.h"

using std::make_shared;
using std::shared_ptr;
using std::string;
using K::Core::ActionInterface;
using K::Core::ThreadPool;
using K::Core::Timers;
using K::IO::ConnectionIO;

namespace K {
namespace Events {

NetworkEventCouplingClient::NetworkEventCouplingClient(
    const string &protocolVersion, const shared_ptr<EventHub> &hub,
    const shared_ptr<ActionInterface> &onConnectedAction, const shared_ptr<ActionInterface> &onFailedToConnectAction,
    const shared_ptr<ActionInterface> &onDisconnectedAction, const shared_ptr<ConnectionIO> &connectionIO,
    const shared_ptr<ThreadPool> &threadPool, const shared_ptr<Timers> &timers)
        : threadPool_(threadPool) {
    sharedState_ = make_shared<SharedState>();
    worker_      = make_shared<Worker>(protocolVersion, hub, onConnectedAction, onFailedToConnectAction,
                                       onDisconnectedAction, sharedState_, connectionIO, threadPool_, timers);
}

NetworkEventCouplingClient::~NetworkEventCouplingClient() {
    Disconnect();
}

void NetworkEventCouplingClient::Connect(const string &host, int port) {
    sharedState_->WaitForWorker();
    sharedState_->PrepareToConnect();
    worker_->SetHost(host, port);
    threadPool_->Run(worker_.get(), sharedState_.get(), workerCompletionId);
}

void NetworkEventCouplingClient::Disconnect() {
    sharedState_->WaitForWorker();
    sharedState_->Disconnect();
}

}    // Namespace Events.
}    // Namespace K.

