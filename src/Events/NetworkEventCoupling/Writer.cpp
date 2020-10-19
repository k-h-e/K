#include "Writer.h"
#include "SharedState.h"

#include <K/Core/Log.h>
#include <K/IO/SocketStream.h>
#include <K/IO/BlockingStreamCore.h>
#include <K/Events/EventLoopHub.h>

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using K::Core::Buffer;
using K::Core::Log;
using K::IO::SocketStream;

namespace K {
namespace Events {

NetworkEventCoupling::Writer::Writer(const shared_ptr<SocketStream> &stream, const shared_ptr<EventLoopHub> &hub,
                                     int hubClientId, shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState),
          stream_(stream),
          hub_(hub),
          hubClientId_(hubClientId) {
    // Nop.
}

void NetworkEventCoupling::Writer::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    unique_ptr<Buffer> buffer(new Buffer());
    while (!stream_->Error() && hub_->GetEvents(hubClientId_, &buffer, false)) {
        int dataSize = buffer->DataSize();
        if (dataSize > 0) {    // Defensive, shouldn't be necessary.
            uint32_t size = dataSize;
            stream_->WriteItem(&size, sizeof(size));
            stream_->WriteItem(buffer->Data(), dataSize);
        }
    }

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace Events.
}    // Namespace K.

