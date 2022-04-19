#include <K/Events/NetworkEventCoupling.h>

#include <K/Core/ThreadPool.h>
#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventHub.h>
#include "SharedState.h"
#include "Writer.h"

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::to_string;
using std::chrono::milliseconds;
using K::Core::CompletionHandlerInterface;
using K::Core::Log;
using K::Core::ThreadPool;
using K::Core::Timers;
using K::IO::TcpConnection;
using K::Events::EventHub;

namespace K {
namespace Events {

NetworkEventCoupling::NetworkEventCoupling(
        const shared_ptr<TcpConnection> &tcpConnection, const string &protocolVersion,
        const shared_ptr<EventHub> &hub, const shared_ptr<CompletionHandlerInterface> &completionHandler,
        int completionId, const std::shared_ptr<ThreadPool> &threadPool, const std::shared_ptr<Timers> &timers) {
    int hubClientId = hub->RegisterEventLoop();
    sharedState_ = make_shared<SharedState>(completionHandler, completionId, hub, hubClientId);
    sharedState_->RegisterTcpConnection(tcpConnection.get());

    writer_ = make_shared<Writer>(tcpConnection, protocolVersion, hub, hubClientId, sharedState_, timers);
    threadPool->Run(writer_.get(), sharedState_.get(), writerCompletionId);
}

NetworkEventCoupling::~NetworkEventCoupling() {
    sharedState_->ShutDown();    // Writer (thread) cleans everything up when it terminates.
}

}    // Namespace Events.
}    // Namespace K.

