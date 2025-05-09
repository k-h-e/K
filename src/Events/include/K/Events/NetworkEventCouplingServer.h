/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_

#include <memory>
#include <string>
#include <K/Core/RunLoop.h>
#include <K/Core/Timer.h>
#include <K/IO/KeepAliveParameters.h>
#include <K/IO/ListenSocket.h>
#include <K/Events/NetworkEventCoupling.h>

namespace K {
    namespace Core {
        class IoBuffers;
        class ThreadPool;
        class Timers;
    }
    namespace IO {
        class ConnectionIO;
    }
    namespace Events {
        class EventFilterConfiguration;
        class EventHub;
    }
}

namespace K {
namespace Events {

//! Installs network event couplings for incoming network connections.
/*!
 *  Only one network event coupling can be installed at a given time. If a new connection comes in when there already is
 *  a coupling installed, the old coupling is uninstalled and a new coupling is installed for the new connection.
 */
class NetworkEventCouplingServer : public virtual K::Core::Interface,
                                   private virtual Core::RunLoop::ClientInterface,
                                   private virtual IO::ListenSocket::HandlerInterface,
                                   private virtual NetworkEventCoupling::HandlerInterface,
                                   private virtual K::Core::Timer::HandlerInterface {
  public:
    class HandlerInterface : public virtual Core::Interface {
      public:
        //! Called when the server has installed a new network event coupling.
        /*!
         *  If the server already had a network event coupling installed, that coupling has been replaced by the new
         *  one.
         */
        virtual void OnNetworkEventCouplingInstalled() = 0;
        //! Called when the server has uninstalled its network event coupling.
        virtual void OnNetworkEventCouplingUninstalled() = 0;
    };

    NetworkEventCouplingServer(
        int port, const std::string &protocolVersion, const IO::KeepAliveParameters &keepAliveParameters,
        const std::shared_ptr<EventHub> &hub, const std::shared_ptr<Event> &connectedEvent,
        const std::shared_ptr<Event> &disconnectedEvent, const std::shared_ptr<Core::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO, const std::shared_ptr<Core::IoBuffers> &ioBuffers,
        const std::shared_ptr<Core::Timers> &timers, const std::shared_ptr<Core::ThreadPool> &threadPool);
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
     */
    void Register(HandlerInterface *handler);
    //! Configures outgoing event filtering.
    /*!
     *  The setting will take effect with the next event coupling that gets installed.
     *
     *  /param configuration
     *  Pass <c>nullptr</c> to disable filtering. 
     */
    void ConfigureOutgoingEventFilter(const std::shared_ptr<EventFilterConfiguration> &configuration);

  private:
    void Activate(bool deepActivation) override;
    void OnListenSocketAcceptedConnection(std::unique_ptr<IO::TcpConnection> connection)
        override;
    void OnListenSocketErrorState() override;
    void OnNetworkEventCouplingErrorState() override;
    void OnTimer() override;

    void InstallListenSocket();
    void UninstallListenSocket();
    void InstallCoupling(std::unique_ptr<IO::TcpConnection> connection);
    void UninstallCoupling();
    void InstallTimer();
    void UninstallTimer();

    const std::shared_ptr<EventHub>         hub_;             // Thread-safe.
    const std::shared_ptr<IO::ConnectionIO> connectionIO_;    // Thread-safe.
    const std::shared_ptr<Core::IoBuffers>  ioBuffers_;       // Thread-safe.
    const std::shared_ptr<Core::Timers>     timers_;          // Thread-safe.
    const std::shared_ptr<Core::ThreadPool> threadPool_;      // Thread-safe.

    const std::shared_ptr<Core::RunLoop>      runLoop_;
    int                                       runLoopClientId_;
    HandlerInterface                          *handler_;
    int                                       port_;
    std::string                               protocolVersion_;
    IO::KeepAliveParameters                   keepAliveParameters_;
    std::shared_ptr<Event>                    connectedEvent_;
    std::shared_ptr<Event>                    disconnectedEvent_;
    std::shared_ptr<EventFilterConfiguration> eventFilterConfiguration_;
    std::unique_ptr<IO::ListenSocket>         listenSocket_;
    std::unique_ptr<NetworkEventCoupling>     coupling_;
    std::unique_ptr<Core::Timer>              timer_;
    bool                                      signalCouplingInstallation_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGSERVER_H_
