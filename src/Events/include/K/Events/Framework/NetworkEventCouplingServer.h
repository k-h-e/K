#ifndef K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_
#define K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_

#include <memory>
#include <string>
#include <K/Core/Interface.h>

namespace K {
    namespace Core {
        class ThreadPool;
        class Timers;
        namespace Framework {
            class RunLoop;
        }
    }
    namespace IO {
        class ConnectionIO;
        class KeepAliveParameters;
    }
    namespace Events {
        class EventLoopHub;
    }
}

namespace K {
namespace Events {
//! Framework- (<c>K::Core::Framework::RunLoop</c>-) enabled variants of event mechanism classes.
namespace Framework {

//! Installs network event couplings for incoming network connections.
class NetworkEventCouplingServer : public virtual K::Core::Interface {
  public:
    NetworkEventCouplingServer(
        int port, const std::string &protocolVersion, const IO::KeepAliveParameters &keepAliveParameters,
        const std::shared_ptr<EventLoopHub> &hub, const std::shared_ptr<K::Core::Framework::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO, const std::shared_ptr<K::Core::Timers> &timers,
        const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    NetworkEventCouplingServer()                                                   = delete;
    NetworkEventCouplingServer(const NetworkEventCouplingServer &other)            = delete;
    NetworkEventCouplingServer &operator=(const NetworkEventCouplingServer &other) = delete;
    NetworkEventCouplingServer(NetworkEventCouplingServer &&other)                 = delete;
    NetworkEventCouplingServer &operator=(NetworkEventCouplingServer &&other)      = delete;
    ~NetworkEventCouplingServer();

  private:
   class Core;

   std::unique_ptr<Core> core_;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_
