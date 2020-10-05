#ifndef K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_

#include <memory>
#include <K/Core/ErrorInterface.h>

namespace K {
namespace Core {
    class ThreadPool;
}
namespace IO {
    class ListenSocket;
}
}

namespace K {
namespace Events {

class EventLoopHub;

//! Installs network event couplings for incoming network connections.
class NetworkEventCouplingServer : public virtual K::Core::ErrorInterface {
  public:
    NetworkEventCouplingServer(int port, const std::shared_ptr<EventLoopHub> &hub,
                               const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    ~NetworkEventCouplingServer();
    bool Error();

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
