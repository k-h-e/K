#ifndef K_EVENTS_NETWORKEVENTCOUPLING_READER_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_READER_H_

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

//! Reader for the network event coupling.
/*!
 *  \ingroup Events
 */
class NetworkEventCoupling::Reader : public virtual K::Core::ActionInterface {
  public:
    Reader(const std::shared_ptr<K::IO::SocketStream> &stream, const std::shared_ptr<EventLoopHub> &hub,
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

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_READER_H_
