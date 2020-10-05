#ifndef K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_

#include <K/Core/ActionInterface.h>
#include <K/Events/NetworkEventCoupling.h>

namespace K {
namespace IO {
    class SocketStream;
}
}

namespace K {
namespace Events {

class EventLoopHub;

//! Writer for the network event coupling.
class NetworkEventCoupling::Writer : public virtual K::Core::ActionInterface {
  public:
    Writer(const std::shared_ptr<K::IO::SocketStream> &stream, const std::shared_ptr<EventLoopHub> &hub,
           int hubClientId, std::shared_ptr<SharedState> sharedState);
    void ExecuteAction();

  private:
    std::shared_ptr<SharedState>         sharedState_;
    std::shared_ptr<K::IO::SocketStream> stream_;
    std::shared_ptr<EventLoopHub>        hub_;

    int hubClientId_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_
