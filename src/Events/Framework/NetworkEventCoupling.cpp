#include <K/Events/Framework/NetworkEventCoupling.h>

#include <K/IO/Framework/TcpConnection.h>

using std::move;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using K::IO::Framework::TcpConnection;

namespace K {
namespace Events {
namespace Framework {

NetworkEventCoupling::NetworkEventCoupling(unique_ptr<TcpConnection> tcpConnection, const string &protocolVersion,
                                           const shared_ptr<EventLoopHub> &hub)
        : hub_(hub),
          tcpConnection_(move(tcpConnection)),
          protocolVersion_(protocolVersion),
          handler_(nullptr),
          handlerAssociatedId_(0),
          readBufferFill_(0),
          error_(false) {
    tcpConnection_->Register(this, 0);
}

NetworkEventCoupling::~NetworkEventCoupling() {
    // TODO: Check desconstruction.
}

void NetworkEventCoupling::Register(HandlerInterface *handler, int id) {
    handler_             = handler;
    handlerAssociatedId_ = handler_ ? id : 0;
}

void NetworkEventCoupling::OnStreamReadyRead(int id) {
    (void)id;
    (void)readBufferFill_;
    while (true) {

    }
}

void NetworkEventCoupling::OnStreamReadyWrite(int id) {
    (void)id;
    puts("vvv");
}

bool NetworkEventCoupling::ErrorState() const {
    return error_;
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.
