#ifndef K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_

#include <K/Core/ActionInterface.h>
#include <K/Events/NetworkEventCoupling.h>

namespace K {
namespace IO {
    class TcpConnection;
}
}

namespace K {
namespace Events {

class EventLoopHub;

//! Writer for the network event coupling.
class NetworkEventCoupling::Writer : public virtual K::Core::ActionInterface {
  public:
    Writer(const std::shared_ptr<K::IO::TcpConnection> &tcpConnection, const std::shared_ptr<EventLoopHub> &hub,
           int hubClientId, const std::shared_ptr<ReadHandler> &readHandler, std::shared_ptr<SharedState> sharedState);
    void ExecuteAction();

  private:
    std::shared_ptr<SharedState>          sharedState_;

    std::shared_ptr<EventLoopHub>         hub_;            // Thread-safe.
    std::shared_ptr<ReadHandler>          readHandler_;    // Given to the TCP connection, called on arbitrary thread.

    std::shared_ptr<K::IO::TcpConnection> tcpConnection_;
    int                                   hubClientId_;
    std::chrono::milliseconds             keepAliveSendInterval_;
    uint32_t                              version_;

};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_
