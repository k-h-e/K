#include <K/Events/NetworkEventCouplingServer.h>

#include "Installer.h"
#include "SharedState.h"

using std::shared_ptr;
using std::string;
using std::make_shared;
using std::chrono::milliseconds;
using K::Core::ThreadPool;
using K::Core::Timers;
using K::IO::ListenSocket;
using K::IO::ConnectionIO;

namespace K {
namespace Events {

NetworkEventCouplingServer::NetworkEventCouplingServer(
        int port, const string &protocolVersion, const shared_ptr<EventLoopHub> &hub,
        const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<ThreadPool> &threadPool,
        const shared_ptr<Timers> &timers)
            : timers_(timers) {
    shared_    = make_shared<SharedState>(protocolVersion, hub, connectionIO, threadPool, timers);
    installer_ = make_shared<Installer>(port, shared_, connectionIO, threadPool);

    timer_ = timers_->AddTimer(milliseconds(5000), installer_.get(), true);
    shared_->SetTimer(timer_);
}

NetworkEventCouplingServer::~NetworkEventCouplingServer() {
    shared_->ShutDown();
    timers_->RemoveTimer(timer_);
}

}    // Namespace Events.
}    // Namespace K.

