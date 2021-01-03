#include "Writer.h"
#include "SharedState.h"

#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventLoopHub.h>

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using K::Core::Buffer;
using K::Core::Log;
using K::IO::TcpConnection;

namespace K {
namespace Events {

NetworkEventCoupling::Writer::Writer(
    const shared_ptr<TcpConnection> &tcpConnection, const shared_ptr<EventLoopHub> &hub, int hubClientId,
    shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState),
          hub_(hub),
          tcpConnection_(tcpConnection),
          hubClientId_(hubClientId) {
    // Nop.
}

void NetworkEventCoupling::Writer::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    unique_ptr<Buffer> buffer(new Buffer());
    while (!tcpConnection_->ErrorState() && hub_->GetEvents(hubClientId_, &buffer, false)) {
        int dataSize = buffer->DataSize();
        if (dataSize > 0) {    // Defensive, shouldn't be necessary.
            uint32_t size = dataSize;
            tcpConnection_->WriteItem(&size, sizeof(size));
            tcpConnection_->WriteItem(buffer->Data(), dataSize);
        }
    }

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace Events.
}    // Namespace K.

