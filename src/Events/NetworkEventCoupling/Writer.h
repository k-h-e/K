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
/*!
 *  When the writer terminates, it shuts everything down. Specifically, it takes over the reponsibility of unregistering
 *  from the event loop hub.
 */
class NetworkEventCoupling::Writer : public virtual K::Core::ActionInterface {
  public:
    Writer(const std::shared_ptr<K::IO::TcpConnection> &tcpConnection, const std::shared_ptr<EventLoopHub> &hub,
           int hubClientId, const std::shared_ptr<SharedState> &sharedState,
           const std::shared_ptr<Core::Timers> &timers);
    void ExecuteAction();

  private:
    std::shared_ptr<SharedState>          sharedState_;    // Thread-safe.

    std::shared_ptr<Core::Timers>         timers_;         // Thread-safe.
    std::shared_ptr<EventLoopHub>         hub_;            // Thread-safe.

    std::shared_ptr<K::IO::TcpConnection> tcpConnection_;
    int                                   hubClientId_;
    std::chrono::milliseconds             keepAliveSendInterval_;
    uint32_t                              version_;

};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_
