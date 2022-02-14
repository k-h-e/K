#ifndef K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_

#include <string>
#include <K/Core/ActionInterface.h>
#include <K/IO/KeepAliveParameters.h>
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
 *  When the writer terminates (finishes its <c>ExecuteAction</c> method), it cleans everything up. Specifically, it
 *  unregisters the TCP connection from the shared state at the appropriate time. Unregistering from the event loop hub
 *  is done in the completion handler in the shared state, though.
 */
class NetworkEventCoupling::Writer : public virtual K::Core::ActionInterface {
  public:
    Writer(const std::shared_ptr<K::IO::TcpConnection> &tcpConnection, const std::string &protocolVersion,
           const std::shared_ptr<EventLoopHub> &hub, int hubClientId, const std::shared_ptr<SharedState> &sharedState,
           const std::shared_ptr<Core::Timers> &timers);
    void ExecuteAction();

  private:
    void SendKeepAliveChunk();
    void SendEventsChunk(const void *data, int dataSize);
    void SendVersionChunk();

    std::shared_ptr<SharedState>       sharedState_;    // Thread-safe.

    std::shared_ptr<Core::Timers>      timers_;         // Thread-safe.
    std::shared_ptr<EventLoopHub>      hub_;            // Thread-safe.

    std::shared_ptr<IO::TcpConnection> tcpConnection_;
    IO::KeepAliveParameters            keepAliveParameters_;
    int                                hubClientId_;
    std::string                        protocolVersion_;

};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_WRITER_H_
