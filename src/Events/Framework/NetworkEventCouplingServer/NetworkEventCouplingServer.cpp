#include <K/Events/Framework/NetworkEventCouplingServer.h>

#include "Core.h"

using std::make_unique;
using std::shared_ptr;
using std::string;
using K::Core::ThreadPool;
using K::Core::Timers;
using K::Core::Framework::RunLoop;
using K::IO::ConnectionIO;
using K::IO::KeepAliveParameters;

namespace K {
namespace Events {
namespace Framework {

NetworkEventCouplingServer::NetworkEventCouplingServer(
        int port, const string &protocolVersion, const KeepAliveParameters &keepAliveParameters,
        const shared_ptr<EventHub> &hub, const shared_ptr<RunLoop> &runLoop,
        const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<Timers> &timers,
        const shared_ptr<ThreadPool> &threadPool)
            : core_(make_unique<Core>(port, protocolVersion, keepAliveParameters, hub, runLoop, connectionIO, timers,
                                      threadPool)) {
    // Nop.
}

NetworkEventCouplingServer::~NetworkEventCouplingServer() {
    // TODO: Check deconstruction.
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

