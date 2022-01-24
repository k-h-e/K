#include "Writer.h"
#include "SharedState.h"

#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventLoopHub.h>
#include "ReadHandler.h"

using std::make_shared;
using std::nullopt;
using std::shared_ptr;
using std::to_string;
using std::unique_ptr;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using K::Core::Buffer;
using K::Core::Log;
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
        steady_clock::time_point lastTime = steady_clock::now();
        milliseconds             elapsed(0);
        bool done = false;
        while (!done) {
            if (tcpConnection_->ErrorState()) {
                Log::Print(Log::Level::Debug, this, []{ return "TCP connection down"; });
                done = true;
            } else {
                bool sendKeepAlive = false;

                steady_clock::time_point now = steady_clock::now();
                if (now < lastTime) {    // Wrap-around paranoia.
                    sendKeepAlive = true;
                    lastTime = now;
                }

                milliseconds delta = duration_cast<milliseconds>(now - lastTime);
                if (delta < milliseconds(0)) {    // Wrap-around paranoia.
                    sendKeepAlive = true;
                    lastTime = now;
                    delta = milliseconds(0);
                }

                elapsed += delta;
                if (elapsed >= keepAliveSendInterval_) {
                    sendKeepAlive = true;
                    while (elapsed >= keepAliveSendInterval_) {
                        elapsed -= keepAliveSendInterval_;
                    }
                } else if (elapsed < milliseconds(0)) {    // Wrap-around paranoia.
                    elapsed = milliseconds(0);
                }

                lastTime = now;

                if (sendKeepAlive) {
                    Log::Print(Log::Level::Debug, this, []{ return "sending keep-alive"; });
                    ChunkType chunkType = ChunkType::KeepAlive;
                    uint32_t  chunkSize = static_cast<uint32_t>(sizeof(chunkType));
                    tcpConnection_->WriteItem(&chunkSize, sizeof(chunkSize));
                    tcpConnection_->WriteItem(&chunkType, sizeof(chunkType));
                }

                bool shutdownRequested = !hub_->GetEvents(hubClientId_, &buffer,
                                                          keepAliveSendInterval_ - elapsed + milliseconds(2));

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

