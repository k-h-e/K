#ifndef K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_WORKER_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_WORKER_H_

#include <K/Core/ActionInterface.h>
#include <K/Events/NetworkEventCouplingClient.h>

namespace K {
namespace Events {

class EventLoopHub;

//! Worker for the network event coupling client.
class NetworkEventCouplingClient::Worker : public virtual Core::ActionInterface {
  public:
    Worker(const std::shared_ptr<EventLoopHub> &hub,
           const std::shared_ptr<K::Core::ActionInterface> &onConnectedAction,
           const std::shared_ptr<K::Core::ActionInterface> &onFailedToConnectAction,
           const std::shared_ptr<K::Core::ActionInterface> &onDisconnectedAction,
           const std::shared_ptr<K::Core::ThreadPool> &threadPool, std::shared_ptr<SharedState> sharedState);
    //! Informs the worker about the host to connect to.
    void SetHost(uint32_t ip4Address, int port);
    void ExecuteAction();

  private:
    std::shared_ptr<SharedState>              sharedState_;
    std::shared_ptr<EventLoopHub>             hub_;
    std::shared_ptr<K::Core::ActionInterface> onConnectedAction_;
    std::shared_ptr<K::Core::ActionInterface> onFailedToConnectAction_;
    std::shared_ptr<K::Core::ActionInterface> onDisconnectedAction_;
    std::shared_ptr<K::Core::ThreadPool>      threadPool_;
    uint32_t                                  hostIp4Address_;
    int                                       hostPort_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_WORKER_H_
