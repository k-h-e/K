#ifndef K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_

#include <memory>
#include <K/Core/Interface.h>

namespace K {
namespace Core {
    class Timers;
    class ThreadPool;
}
namespace IO {
    class ConnectionIO;
}
}

namespace K {
namespace Events {

class EventLoopHub;

//! Installs network event couplings for incoming network connections.
class NetworkEventCouplingServer : public virtual Core::Interface {
  public:
    NetworkEventCouplingServer(
        int port, const std::string &protocolVersion, const std::shared_ptr<EventLoopHub> &hub,
        const std::shared_ptr<K::IO::ConnectionIO> &connectionIO,
        const std::shared_ptr<K::Core::ThreadPool> &threadPool, const std::shared_ptr<K::Core::Timers> &timers);
    NetworkEventCouplingServer()                                                   = delete;
    NetworkEventCouplingServer(const NetworkEventCouplingServer &other)            = delete;
    NetworkEventCouplingServer &operator=(const NetworkEventCouplingServer &other) = delete;
    NetworkEventCouplingServer(NetworkEventCouplingServer &&other)                 = delete;
    NetworkEventCouplingServer &operator=(NetworkEventCouplingServer &&other)      = delete;
    ~NetworkEventCouplingServer();

  private:
    class Installer;
    class SharedState;

    std::shared_ptr<Installer>             installer_;    // Thread safe.
    std::shared_ptr<SharedState>           shared_;       // Thread safe.
    const std::shared_ptr<K::Core::Timers> timers_;       // Thread safe.

    int                                    timer_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_
