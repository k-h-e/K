#ifndef K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_

#include <memory>
#include <K/Core/ErrorStateInterface.h>

namespace K {
namespace Core {
    class Timers;
    class ThreadPool;
}
namespace IO {
    class ListenSocket;
    class ConnectionIO;
}
}

namespace K {
namespace Events {

class EventLoopHub;

//! Installs network event couplings for incoming network connections.
class NetworkEventCouplingServer : public virtual K::Core::ErrorStateInterface {
  public:
    NetworkEventCouplingServer(
        int port, const std::string &protocolVersion, const std::shared_ptr<EventLoopHub> &hub,
        const std::shared_ptr<K::IO::ConnectionIO> &connectionIO,
        const std::shared_ptr<K::Core::ThreadPool> &threadPool, const std::shared_ptr<K::Core::Timers> &timers);
    ~NetworkEventCouplingServer();

    bool ErrorState() override;

  private:
    static const int couplingCompletionId = 0;
    static const int workerCompletionId   = 1;

    class SharedState;
    class Worker;

    std::shared_ptr<SharedState>         sharedState_;

    std::shared_ptr<K::IO::ListenSocket> listenSocket_;
    std::shared_ptr<Worker>              worker_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_
