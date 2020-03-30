#include <K/Events/NetworkEventCouplingClient.h>

#include <K/Core/ThreadPool.h>
#include "SharedState.h"
#include "Worker.h"

using std::shared_ptr;
using std::make_shared;
using K::Core::ThreadPool;
using K::Core::ActionInterface;

namespace K {
namespace Events {

NetworkEventCouplingClient::NetworkEventCouplingClient(
    const shared_ptr<EventLoopHub> &hub, const shared_ptr<ActionInterface> &onConnectedAction,
    const shared_ptr<ActionInterface> &onFailedToConnectAction, const shared_ptr<ActionInterface> &onDisconnectedAction,
    const shared_ptr<ThreadPool> &threadPool)
        : threadPool_(threadPool) {
    sharedState_ = make_shared<SharedState>();
    worker_      = make_shared<Worker>(hub, onConnectedAction, onFailedToConnectAction, onDisconnectedAction,
                                       threadPool_, sharedState_);
}

NetworkEventCouplingClient::~NetworkEventCouplingClient() {
    Disconnect();
}

void NetworkEventCouplingClient::Connect(uint32_t ip4Address, int port) {
    sharedState_->WaitForWorker();
    sharedState_->PrepareToConnect();
    worker_->SetHost(ip4Address, port);
    threadPool_->Run(worker_, sharedState_, workerCompletionId);
}

void NetworkEventCouplingClient::Disconnect() {
    sharedState_->WaitForWorker();
    sharedState_->Disconnect();
}

}    // Namespace Events.
}    // Namespace K.

