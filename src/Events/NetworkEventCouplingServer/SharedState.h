#ifndef K_EVENTS_NETWORKEVENTCOUPLINGSERVER_SHAREDSTATE_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGSERVER_SHAREDSTATE_H_

#include <mutex>
#include <optional>
#include <K/IO/ListenSocket.h>
#include <K/Events/NetworkEventCouplingServer.h>

namespace K {
namespace Events {

class NetworkEventCoupling;

//! State shared between threads.
/*!
 *  The class is thread-safe (i.e. all public methods).
 */
class NetworkEventCouplingServer::SharedState : public virtual IO::ListenSocket::HandlerInterface {
  public:
    SharedState(const std::string &protocolVersion, const std::shared_ptr<EventLoopHub> &hub,
                const std::shared_ptr<IO::ConnectionIO> &connectionIO,
                const std::shared_ptr<Core::ThreadPool> &threadPool, const std::shared_ptr<Core::Timers> &timers);
    SharedState()                                    = delete;
    SharedState(const SharedState &other)            = delete;
    SharedState &operator=(const SharedState &other) = delete;
    SharedState(SharedState &&other)                 = delete;
    SharedState &operator=(SharedState &&other)      = delete;

    void SetTimer(int timer);
    void ShutDown();
    void OnNetworkConnectionAccepted(const std::shared_ptr<IO::TcpConnection> &connection) override;
    void OnListenSocketErrorState() override;

  private:
    const std::shared_ptr<EventLoopHub>           hub_;             // Thread safe.
    const std::shared_ptr<IO::ConnectionIO>       connectionIO_;    // Thread safe.
    const std::shared_ptr<Core::ThreadPool>       threadPool_;      // Thread safe.
    const std::shared_ptr<Core::Timers>           timers_;          // Thread safe.

    std::mutex                                    lock_;    // Protects everything below...
    std::string                                   protocolVersion_;
    std::shared_ptr<Events::NetworkEventCoupling> coupling_;
    bool                                          unpauseTimer_;
    std::optional<int>                            timer_;
    bool                                          shutDown_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGSERVER_SHAREDSTATE_H_
