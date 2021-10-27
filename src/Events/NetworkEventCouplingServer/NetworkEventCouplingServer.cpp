#include <K/Events/NetworkEventCouplingServer.h>

#include <K/Core/ThreadPool.h>
#include <K/IO/ListenSocket.h>
#include "SharedState.h"
#include "Worker.h"

using std::shared_ptr;
using std::make_shared;
using K::Core::ThreadPool;
using K::IO::ListenSocket;
using K::IO::ConnectionIO;

namespace K {
namespace Events {

NetworkEventCouplingServer::NetworkEventCouplingServer(
        int port, const shared_ptr<EventLoopHub> &hub, const shared_ptr<ConnectionIO> &connectionIO,
        const shared_ptr<ThreadPool> &threadPool) {
    sharedState_  = make_shared<SharedState>();

    listenSocket_ = make_shared<ListenSocket>(port, connectionIO, threadPool);
    worker_       = make_shared<Worker>(listenSocket_, hub, threadPool, sharedState_);

    threadPool->Run(worker_.get(), sharedState_.get(), workerCompletionId);
}

NetworkEventCouplingServer::~NetworkEventCouplingServer() {
    sharedState_->RequestShutDown();
    listenSocket_->ShutDown();
    sharedState_->WaitForWorkerFinished();
}

bool NetworkEventCouplingServer::ErrorState() {
    return listenSocket_->ErrorState();
}

}    // Namespace Events.
}    // Namespace K.

