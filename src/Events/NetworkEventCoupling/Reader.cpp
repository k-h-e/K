#include "Reader.h"
#include "SharedState.h"

#include <K/Core/Buffer.h>
#include <K/Core/Log.h>
#include <K/IO/SocketStream.h>
#include <K/IO/BlockingStreamCore.h>
#include <K/Events/EventLoopHub.h>

using std::shared_ptr;
using K::Core::Log;
using K::Core::Buffer;
using K::IO::SocketStream;

namespace K {
namespace Events {

NetworkEventCoupling::Reader::Reader(const shared_ptr<SocketStream> &stream, const shared_ptr<EventLoopHub> &hub,
                                     int hubClientId, shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState),
          stream_(stream),
          hub_(hub),
          hubClientId_(hubClientId) {
    // Nop.
}

void NetworkEventCoupling::Reader::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    Buffer buffer;
    bool   badSize = false;
    while (!stream_->Error() && !stream_->Eof() && !badSize) {
        uint32_t size;
        if (stream_->ReadItem(&size, sizeof(size))) {
            int sizeAsInt = static_cast<int>(size);
            if (sizeAsInt <= 0) {
                badSize = true;
            }
            if (!badSize) {
                buffer.Clear();
                buffer.Append(nullptr, sizeAsInt);
                if (stream_->ReadItem(buffer.Data(), buffer.DataSize())) {
                    hub_->Post(hubClientId_, buffer, true);
                }
            }
        }
    }

    hub_->RequestShutDown(hubClientId_);

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace Events.
}    // Namespace K.

