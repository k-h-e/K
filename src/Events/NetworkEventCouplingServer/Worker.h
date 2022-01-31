#ifndef K_EVENTS_NETWORKEVENTCOUPLINGSERVER_WORKER_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGSERVER_WORKER_H_

#include <K/Core/ActionInterface.h>
#include <K/Events/NetworkEventCouplingServer.h>

namespace K {
namespace Core {
    class Timers;
}
namespace IO {
    class ListenSocket;
}
}

namespace K {
namespace Events {

class EventLoopHub;

//! Worker for the network event coupling server.
class NetworkEventCouplingServer::Worker : public virtual K::Core::ActionInterface {
  public:
    Worker(const std::shared_ptr<K::IO::ListenSocket> &listenSocket, const std::shared_ptr<EventLoopHub> &hub,
           const std::shared_ptr<SharedState> &sharedState, const std::shared_ptr<K::Core::ThreadPool> &threadPool,
           const std::shared_ptr<K::Core::Timers> &timers);
    void ExecuteAction();

  private:
    std::shared_ptr<SharedState>         sharedState_;

    std::shared_ptr<K::IO::ListenSocket> listenSocket_;
    std::shared_ptr<EventLoopHub>        hub_;
    std::shared_ptr<K::Core::ThreadPool> threadPool_;
    std::shared_ptr<K::Core::Timers>     timers_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGSERVER_WORKER_H_
