#include "Writer.h"

#include <K/Core/Log.h>
#include <K/Core/PolledCyclicTrigger.h>
#include <K/Core/StringTools.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventLoopHub.h>
#include "ReadHandler.h"
#include "SharedState.h"

using std::make_shared;
using std::nullopt;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;
using std::chrono::milliseconds;
using K::Core::Buffer;
using K::Core::Log;
using K::Core::PolledCyclicTrigger;
using K::Core::StringTools;
using K::Core::Timers;
using K::IO::TcpConnection;

namespace K {
namespace Events {

NetworkEventCoupling::Writer::Writer(
    const shared_ptr<TcpConnection> &tcpConnection, const string &protocolVersion, const shared_ptr<EventLoopHub> &hub,
    int hubClientId, const shared_ptr<SharedState> &sharedState, const shared_ptr<Timers> &timers)
        : sharedState_(sharedState),
          timers_(timers),
          hub_(hub),
          tcpConnection_(tcpConnection),
          keepAliveParameters_(milliseconds(1000), milliseconds(8000)),
          hubClientId_(hubClientId),
          protocolVersion_(protocolVersion) {
    // Nop.
}

void NetworkEventCoupling::Writer::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    auto readHandler = make_shared<ReadHandler>(protocolVersion_, hub_, hubClientId_, sharedState_);
    if (tcpConnection_->Register(readHandler)) {
        int timer = timers_->AddTimer(keepAliveParameters_.CheckInterval(), sharedState_.get(), false);

        unique_ptr<Buffer>  buffer(new Buffer());
        PolledCyclicTrigger sendKeepAliveTrigger(keepAliveParameters_.SendInterval());

        SendVersionChunk();

        bool done = false;
        while (!done) {
            if (tcpConnection_->WriteFailed()) {
                Log::Print(Log::Level::Debug, this, []{ return "TCP connection down"; });
                done = true;
            } else {
                if (sendKeepAliveTrigger.Check()) {
                    SendKeepAliveChunk();
                    (void)sendKeepAliveTrigger.Check();
                }

                milliseconds timeout = sendKeepAliveTrigger.Remaining() + milliseconds(2);
                if (!(timeout > sendKeepAliveTrigger.Remaining())) {
                    timeout = sendKeepAliveTrigger.Remaining();
                }

                bool shutdownRequested = !hub_->GetEvents(hubClientId_, &buffer, timeout);
                if (shutdownRequested) {
                    Log::Print(Log::Level::Debug, this, []{ return "event hub issued shutdown"; });
                    done = true;
                } else {
                    if (buffer->DataSize() > 0) {
                        SendEventsChunk(buffer->Data(), buffer->DataSize());
                    }
                }
            }
        }

        timers_->RemoveTimer(timer);
        tcpConnection_->Unregister(readHandler);
    }

    readHandler.reset();
    sharedState_->RegisterTcpConnection(nullptr);
    tcpConnection_.reset();

    // Unregistering from the event loop hub is done in the completion handler in the shared state.

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

void NetworkEventCoupling::Writer::SendKeepAliveChunk() {
    ChunkType chunkType = ChunkType::KeepAlive;
    uint32_t  chunkSize = static_cast<uint32_t>(sizeof(chunkType));
    tcpConnection_->WriteItem(&chunkSize, sizeof(chunkSize));
    tcpConnection_->WriteItem(&chunkType, sizeof(chunkType));
}

void NetworkEventCoupling::Writer::SendEventsChunk(const void *data, int dataSize) {
    ChunkType chunkType = ChunkType::Events;
    uint32_t  chunkSize = static_cast<uint32_t>(dataSize) + static_cast<uint32_t>(sizeof(chunkType));
    tcpConnection_->WriteItem(&chunkSize, sizeof(chunkSize));
    tcpConnection_->WriteItem(&chunkType, sizeof(chunkType));
    tcpConnection_->WriteItem(data, dataSize);
}

void NetworkEventCoupling::Writer::SendVersionChunk() {
    vector<uint8_t> versionBinary;
    StringTools::Serialize(protocolVersion_, &versionBinary);

    ChunkType chunkType = ChunkType::Version;
    uint32_t  chunkSize = static_cast<uint32_t>(sizeof(chunkType) + versionBinary.size());
    tcpConnection_->WriteItem(&chunkSize, sizeof(chunkSize));
    tcpConnection_->WriteItem(&chunkType, sizeof(chunkType));
    tcpConnection_->WriteItem(&versionBinary[0], static_cast<int>(versionBinary.size()));
}

}    // Namespace Events.
}    // Namespace K.

