#include "Writer.h"

#include <K/Core/Log.h>
#include <K/Core/StopWatch.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventLoopHub.h>
#include "ReadHandler.h"
#include "SharedState.h"

using std::make_shared;
using std::nullopt;
using std::shared_ptr;
using std::to_string;
using std::unique_ptr;
using std::chrono::milliseconds;
using K::Core::Buffer;
using K::Core::Log;
using K::Core::StopWatch;
using K::Core::Timers;
using K::IO::TcpConnection;

namespace K {
namespace Events {

NetworkEventCoupling::Writer::Writer(
    const shared_ptr<TcpConnection> &tcpConnection, const shared_ptr<EventLoopHub> &hub, int hubClientId,
    const shared_ptr<SharedState> &sharedState, const shared_ptr<Timers> &timers)
        : sharedState_(sharedState),
          timers_(timers),
          hub_(hub),
          tcpConnection_(tcpConnection),
          hubClientId_(hubClientId),
          keepAliveSendInterval_(1000),
          version_(1u) {
    // Nop.
}

void NetworkEventCoupling::Writer::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    auto readHandler = make_shared<ReadHandler>(hub_, hubClientId_, sharedState_);
        // Shared state will outlive the read handler. We see to it in this scope!
    if (tcpConnection_ && tcpConnection_->Register(readHandler)) {
        sharedState_->RegisterTcpConnection(tcpConnection_.get());
        int timer = timers_->AddTimer(milliseconds(8000), sharedState_.get());

        unique_ptr<Buffer> buffer(new Buffer());
        StopWatch    stopWatch;
        milliseconds timeout;
        bool done = false;
        while (!done) {
            if (tcpConnection_->ErrorState()) {
                Log::Print(Log::Level::Debug, this, []{ return "TCP connection down"; });
                done = true;
            } else {
                if (stopWatch.CyclicCheck(keepAliveSendInterval_, &timeout)) {
                    ChunkType chunkType = ChunkType::KeepAlive;
                    uint32_t  chunkSize = static_cast<uint32_t>(sizeof(chunkType));
                    tcpConnection_->WriteItem(&chunkSize, sizeof(chunkSize));
                    tcpConnection_->WriteItem(&chunkType, sizeof(chunkType));

                    (void)stopWatch.CyclicCheck(keepAliveSendInterval_, &timeout);
                }

                bool shutdownRequested = !hub_->GetEvents(hubClientId_, &buffer, timeout + milliseconds(2));

                if (shutdownRequested) {
                    Log::Print(Log::Level::Debug, this, []{ return "event hub issued shutdown"; });
                    done = true;
                } else {
                    int dataSize = buffer->DataSize();
                    if (dataSize > 0) {
                        ChunkType chunkType = ChunkType::Events;
                        uint32_t  chunkSize = static_cast<uint32_t>(dataSize)
                                                  + static_cast<uint32_t>(sizeof(chunkType) + sizeof(version_));
                        tcpConnection_->WriteItem(&chunkSize, sizeof(chunkSize));
                        tcpConnection_->WriteItem(&chunkType, sizeof(chunkType));
                        tcpConnection_->WriteItem(&version_, sizeof(version_));
                        tcpConnection_->WriteItem(buffer->Data(), dataSize);
                    }
                }
            }
        }

        timers_->RemoveTimer(timer);
        sharedState_->RegisterTcpConnection(nullptr);
        tcpConnection_->Unregister(readHandler);
    }

    tcpConnection_.reset();
    hub_->UnregisterEventLoop(hubClientId_);

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace Events.
}    // Namespace K.

