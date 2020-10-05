#ifndef K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_H_

#include <memory>

namespace K {
namespace Core {
    class ThreadPool;
    class ActionInterface;
}
}

namespace K {
namespace Events {

class EventLoopHub;

//! Establishes network connections (at most one at a time) to remote event loop mechanisms and installs local network
//! event couplings for them.
class NetworkEventCouplingClient {
  public:
    //! Constructor.
    /*!
     *  \param onConnectedAction
     *  Optional. If present, gets called on an arbitrary thread when a connection has successfully been established.
     *
     *  \param onFailedToConnectAction
     *  Optional. If present, gets called on an arbitrary thread when an attempt to establish a connection has failed.
     *
     *  \param onDisconnectedAction
     *  Optional. If present, gets called on an arbitrary thread when a connection has been closed or has broken
     *  down.
     */
    NetworkEventCouplingClient(
        const std::shared_ptr<EventLoopHub> &hub,
        const std::shared_ptr<K::Core::ActionInterface> &onConnectedAction,
        const std::shared_ptr<K::Core::ActionInterface> &onFailedToConnectAction,
        const std::shared_ptr<K::Core::ActionInterface> &onDisconnectedAction,
        const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    ~NetworkEventCouplingClient();
    //! Establishes a network event coupling connection to the specified host.
    /*!
     *  The connection is established in the background, so when this method returns this does not mean that the
     *  connection is already up.
     */
    void Connect(uint32_t ip4Address, int port);
    //! Disconnects the network event coupling connection (if one is present or in the process of being established).
    void Disconnect();

  private:
    static const int workerCompletionId = 0;
    class SharedState;
    class Worker;

    std::shared_ptr<SharedState>         sharedState_;

    std::shared_ptr<Worker>              worker_;
    std::shared_ptr<K::Core::ThreadPool> threadPool_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_H_
