#ifndef K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_
#define K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_

#include <memory>
#include <string>
#include <K/Core/Framework/RunLoop.h>
#include <K/Core/Framework/Timer.h>
#include <K/IO/KeepAliveParameters.h>
#include <K/IO/Framework/ListenSocket.h>
#include <K/Events/Framework/NetworkEventCoupling.h>

namespace K {
    namespace Core {
        class ThreadPool;
        class Timers;
    }
    namespace IO {
        class ConnectionIO;
    }
    namespace Events {
        class EventHub;
    }
}

namespace K {
namespace Events {
//! Framework- (<c>K::Core::Framework::RunLoop</c>-) enabled variants of event mechanism classes.
namespace Framework {

//! Installs network event couplings for incoming network connections.
/*!
 *  Only one network event coupling can be installed at a given time. If a new connection comes in when there already is
 *  a coupling installed, the old coupling is uninstalled and a new coupling is installed for the new connection.
 */
class NetworkEventCouplingServer : public virtual K::Core::Interface,
                                   private virtual Core::Framework::RunLoop::ClientInterface,
                                   private virtual IO::Framework::ListenSocket::HandlerInterface,
                                   private virtual Events::Framework::NetworkEventCoupling::HandlerInterface,
                                   private virtual K::Core::Framework::Timer::HandlerInterface {
  public:
    class HandlerInterface : public virtual Core::Interface {
      public:
        //! Called when the server has installed a new network event coupling.
        /*!
         *  If the server already had a network event coupling installed, that coupling has been replaced by the new
         *  one.
         *
         *  \param id
         *  ID that was given when the handler was registered.
         */
        virtual void OnNetworkEventCouplingInstalled(int id) = 0;
        //! Called when the server has uninstalled its network event coupling.
        /*!
         *  \param id
         *  ID that was given when the handler was registered.
         */
        virtual void OnNetworkEventCouplingUninstalled(int id) = 0;
    };

    NetworkEventCouplingServer(
        int port, const std::string &protocolVersion, const IO::KeepAliveParameters &keepAliveParameters,
        const std::shared_ptr<EventHub> &hub, const std::shared_ptr<K::Core::Framework::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO, const std::shared_ptr<K::Core::Timers> &timers,
        const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    NetworkEventCouplingServer()                                                   = delete;
    NetworkEventCouplingServer(const NetworkEventCouplingServer &other)            = delete;
    NetworkEventCouplingServer &operator=(const NetworkEventCouplingServer &other) = delete;
    NetworkEventCouplingServer(NetworkEventCouplingServer &&other)                 = delete;
    NetworkEventCouplingServer &operator=(NetworkEventCouplingServer &&other)      = delete;
    ~NetworkEventCouplingServer();

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the network
     *  event coupling server.
     *
     *  The handler is expected to outlive the network event coupling server. The handler will not get called upon
     *  destruction of the network event coupling server.
     *
     *  \param id
     *  ID to be passed along with handler activations for the network event coupling server. Useful in case one wants
     *  to use one handler with multiple network event coupling server.
     */
    void Register(HandlerInterface *handler, int id);

  private:
    void Activate(bool deepActivation) override;
    void OnListenSocketAcceptedConnection(int id, std::unique_ptr<IO::Framework::TcpConnection> connection)
        override;
    void OnListenSocketErrorState(int id) override;
    void OnNetworkEventCouplingErrorState(int id) override;
    void OnTimer(int id) override;

    void InstallListenSocket();
    void UninstallListenSocket();
    void InstallCoupling(std::unique_ptr<IO::Framework::TcpConnection> connection);
    void UninstallCoupling();
    void InstallTimer();
    void UninstallTimer();

    const std::shared_ptr<EventHub>            hub_;             // Thread safe.
    const std::shared_ptr<IO::ConnectionIO>    connectionIO_;    // Thread safe.
    const std::shared_ptr<K::Core::Timers>     timers_;          // Thread safe.
    const std::shared_ptr<K::Core::ThreadPool> threadPool_;      // Thread safe.

    const std::shared_ptr<K::Core::Framework::RunLoop> runLoop_;
    int                                                runLoopClientId_;
    HandlerInterface                                   *handler_;
    int                                                handlerActivationId_;
    int                                                port_;
    std::string                                        protocolVersion_;
    IO::KeepAliveParameters                            keepAliveParameters_;
    std::unique_ptr<IO::Framework::ListenSocket>       listenSocket_;
    std::unique_ptr<NetworkEventCoupling>              coupling_;
    std::unique_ptr<K::Core::Framework::Timer>         timer_;
    bool                                               signalCouplingInstallation_;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_
