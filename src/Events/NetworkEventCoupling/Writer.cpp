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
using K::IO::TcpConnection;

namespace K {
namespace Events {

NetworkEventCoupling::Writer::Writer(
    const shared_ptr<TcpConnection> &tcpConnection, const shared_ptr<EventLoopHub> &hub, int hubClientId,
    const shared_ptr<ReadHandler> &readHandler, shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState),
          hub_(hub),
          readHandler_(readHandler),
          tcpConnection_(tcpConnection),
          hubClientId_(hubClientId),
          keepAliveSendInterval_(1000),
          version_(1u) {
    // Nop.
}

void NetworkEventCoupling::Writer::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    if (tcpConnection_ && tcpConnection_->Register(readHandler_)) {
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
                    Log::Print(Log::Level::Debug, this, []{ return "sending keep-alive"; });
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
                        Log::Print(Log::Level::Debug, this, [&]{
                            return "sending event data, size=" + to_string(dataSize);
                        });
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

        tcpConnection_->Unregister(readHandler_);
    }

    tcpConnection_.reset();

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace Events.
}    // Namespace K.

