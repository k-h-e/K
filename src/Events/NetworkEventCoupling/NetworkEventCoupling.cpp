#include <K/Events/NetworkEventCoupling.h>

#include <K/Core/ThreadPool.h>
#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventLoopHub.h>
#include "SharedState.h"
#include "Writer.h"
#include "ReadHandler.h"

using std::shared_ptr;
using std::make_shared;
using std::to_string;
using K::Core::CompletionHandlerInterface;
using K::Core::ThreadPool;
using K::Core::Log;
using K::IO::TcpConnection;
using K::Events::EventLoopHub;

namespace K {
namespace Events {

NetworkEventCoupling::NetworkEventCoupling(
    const shared_ptr<TcpConnection> &tcpConnection, const shared_ptr<EventLoopHub> &hub,
    const shared_ptr<CompletionHandlerInterface> &completionHandler, int completionId,
    const std::shared_ptr<K::Core::ThreadPool> &threadPool)
        : hub_(hub) {
    sharedState_ = make_shared<SharedState>(completionHandler, completionId);

    hubClientId_ = hub_->RegisterEventLoop();

    auto readHandler = make_shared<ReadHandler>(hub_, hubClientId_);

    writer_ = make_shared<Writer>(tcpConnection, hub_, hubClientId_, readHandler, sharedState_);
    threadPool->Run(writer_.get(), sharedState_.get(), writerCompletionId);
}

NetworkEventCoupling::~NetworkEventCoupling() {
    Log::Print(Log::Level::Debug, this, []{ return "shutting down..."; });

    hub_->RequestShutDown(hubClientId_);
    sharedState_->WaitForWriterFinished();

    hub_->UnregisterEventLoop(hubClientId_);
}

}    // Namespace Events.
}    // Namespace K.

