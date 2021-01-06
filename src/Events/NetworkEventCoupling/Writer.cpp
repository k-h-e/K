#include "Writer.h"
#include "SharedState.h"

#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventLoopHub.h>
#include "ReadHandler.h"

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
    const shared_ptr<ReadHandler> &readHandler, shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState),
          hub_(hub),
          readHandler_(readHandler),
          tcpConnection_(tcpConnection),
          hubClientId_(hubClientId) {
    tcpConnection_->Register(readHandler_.get());
}

void NetworkEventCoupling::Writer::ExecuteAction() {
    if (!tcpConnection_) {
        return;
    }

    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    unique_ptr<Buffer> buffer(new Buffer());
    bool done = false;
    while (!done) {
        if (tcpConnection_->ErrorState()) {
            Log::Print(Log::Level::Debug, this, []{ return "TCP connection down"; });
            done = true;
        }
        else {
            if (hub_->GetEvents(hubClientId_, &buffer, false)) {
                int dataSize = buffer->DataSize();
                if (dataSize > 0) {    // Defensive, shouldn't be necessary.
                    uint32_t size = dataSize;
                    tcpConnection_->WriteItem(&size, sizeof(size));
                    tcpConnection_->WriteItem(buffer->Data(), dataSize);
                }
            }
            else {
                Log::Print(Log::Level::Debug, this, []{ return "event hub issued shutdown"; });
                done = true;
            }
        }
    }

    tcpConnection_->Register(nullptr);
    tcpConnection_.reset();

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace Events.
}    // Namespace K.

