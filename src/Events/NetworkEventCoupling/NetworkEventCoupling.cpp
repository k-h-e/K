#include <K/Events/NetworkEventCoupling.h>

#include <K/Core/ThreadPool.h>
#include <K/Core/Log.h>
#include <K/IO/SocketStream.h>
#include <K/Events/EventLoopHub.h>
#include "SharedState.h"
#include "Reader.h"
#include "Writer.h"

using std::shared_ptr;
using std::make_shared;
using K::Core::CompletionHandlerInterface;
using K::Core::ThreadPool;
using K::Core::Log;
using K::IO::SocketStream;

namespace K {
namespace Events {

NetworkEventCoupling::NetworkEventCoupling(
        const shared_ptr<SocketStream> &stream, const shared_ptr<EventLoopHub> &hub,
        const shared_ptr<CompletionHandlerInterface> &completionHandler, int completionId,
        const std::shared_ptr<K::Core::ThreadPool> &threadPool) {
    sharedState_ = make_shared<SharedState>(completionHandler, completionId);

    stream_      = stream;
    hub_         = hub;

    hubClientId_ = hub_->RegisterEventLoop();
    reader_      = make_shared<Reader>(stream_, hub_, hubClientId_, sharedState_);
    writer_      = make_shared<Writer>(stream_, hub_, hubClientId_, sharedState_);

    threadPool->Run(reader_, sharedState_, readerCompletionId);
    threadPool->Run(writer_, sharedState_, writerCompletionId);

    Log::Print(Log::Level::Debug, this, []{ return std::string("installed"); });
}

NetworkEventCoupling::~NetworkEventCoupling() {
    Log::Print(Log::Level::Debug, this, []{ return "shutting down..."; });
    stream_->ShutDown();
    hub_->RequestShutDown(hubClientId_);
    sharedState_->WaitForThreadsFinished();
    hub_->UnregisterEventLoop(hubClientId_);
    Log::Print(Log::Level::Debug, this, []{ return "uninstalled"; });
}

}    // Namespace Events.
}    // Namespace K.

