#include <K/Events/Framework/NetworkEventCoupling.h>

#include <K/Core/Log.h>
#include <K/Core/StringTools.h>
#include <K/Core/Framework/Timer.h>
#include <K/IO/Framework/TcpConnection.h>
#include <K/Events/EventHub.h>
#include <K/Events/Framework/EventNotifier.h>
#include "Core.h"

using std::make_unique;
using std::move;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;
using K::Core::Buffer;
using K::Core::Log;
using K::Core::StringTools;
using K::Core::Timers;
using K::Core::Framework::Timer;
using K::Core::Framework::RunLoop;
using K::IO::KeepAliveParameters;
using K::IO::Framework::TcpConnection;

namespace K {
namespace Events {
namespace Framework {

NetworkEventCoupling::NetworkEventCoupling(
    unique_ptr<TcpConnection> tcpConnection, const string &protocolVersion,
    const KeepAliveParameters &keepAliveParameters, const shared_ptr<EventHub> &hub,
    const shared_ptr<RunLoop> &runLoop, const shared_ptr<Timers> &timers)
        : core_(make_unique<Core>(move(tcpConnection), protocolVersion, keepAliveParameters, hub, runLoop, timers)) {
    // Nop.
}

NetworkEventCoupling::~NetworkEventCoupling() {
    // Nop.
}

void NetworkEventCoupling::Register(HandlerInterface *handler, int id) {
    core_->Register(handler, id);
}

bool NetworkEventCoupling::ErrorState() const {
    return core_->ErrorState();
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.
