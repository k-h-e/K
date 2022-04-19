#ifndef K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_WORKER_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_WORKER_H_

#include <string>
#include <K/Core/ActionInterface.h>
#include <K/Events/NetworkEventCouplingClient.h>

namespace K {
namespace Events {

class EventLoopHub;

//! Worker for the network event coupling client.
class NetworkEventCouplingClient::Worker : public virtual Core::ActionInterface {
  public:
    Worker(
       const std::string &protocolVersion, const std::shared_ptr<EventHub> &hub,
       const std::shared_ptr<K::Core::ActionInterface> &onConnectedAction,
       const std::shared_ptr<K::Core::ActionInterface> &onFailedToConnectAction,
       const std::shared_ptr<K::Core::ActionInterface> &onDisconnectedAction,
       const std::shared_ptr<SharedState> &sharedState, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO,
       const std::shared_ptr<K::Core::ThreadPool> &threadPool, const std::shared_ptr<K::Core::Timers> &timers);
    //! Informs the worker about the host to connect to.
    void SetHost(const std::string &host, int port);
    void ExecuteAction();

  private:
    std::shared_ptr<SharedState>         sharedState_;     // Thread safe.
    std::shared_ptr<EventHub>            hub_;             // Thread safe.
    std::shared_ptr<K::IO::ConnectionIO> connectionIO_;    // Thread safe.
    std::shared_ptr<K::Core::ThreadPool> threadPool_;      // Thread safe.
    std::shared_ptr<K::Core::Timers>     timers_;          // Thread safe.

    std::shared_ptr<K::Core::ActionInterface> onConnectedAction_;
    std::shared_ptr<K::Core::ActionInterface> onFailedToConnectAction_;
    std::shared_ptr<K::Core::ActionInterface> onDisconnectedAction_;
    std::string                               host_;
    int                                       hostPort_;
    std::string                               protocolVersion_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_WORKER_H_
