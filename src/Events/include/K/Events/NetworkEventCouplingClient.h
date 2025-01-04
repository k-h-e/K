/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_H_

#include <memory>
#include <string>
#include <K/Core/Timer.h>
#include <K/IO/KeepAliveParameters.h>
#include <K/IO/TcpConnector.h>
#include <K/Events/NetworkEventCoupling.h>

namespace K {
    namespace Core {
        class IoBuffers;
        class Timers;
        class ThreadPool;
        class RunLoop;
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

//! Establishes a network connection to a remote event loop mechanism and installs a local network event coupling for
//! it.
/*!
 *  If at some point the network connection breaks down, the client will automatically try to reconnect.
 */
class NetworkEventCouplingClient : public virtual Core::Interface,
                                   private virtual IO::Deprecated::TcpConnector::HandlerInterface,
                                   private virtual NetworkEventCoupling::HandlerInterface,
                                   private virtual Core::Timer::HandlerInterface {
  public:
    NetworkEventCouplingClient(
        const std::string &protocolVersion, const IO::KeepAliveParameters &keepAliveParameters,
        const std::shared_ptr<EventHub> &hub, const std::shared_ptr<Event> &connectedEvent,
        const std::shared_ptr<Event> &disconnectedEvent, const std::shared_ptr<K::Core::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO, const std::shared_ptr<Core::IoBuffers> &ioBuffers,
        const std::shared_ptr<K::Core::Timers> &timers, const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    NetworkEventCouplingClient()                                                   = delete;
    NetworkEventCouplingClient(const NetworkEventCouplingClient &other)            = delete;
    NetworkEventCouplingClient &operator=(const NetworkEventCouplingClient &other) = delete;
    NetworkEventCouplingClient(NetworkEventCouplingClient &&other)                 = delete;
    NetworkEventCouplingClient &operator=(NetworkEventCouplingClient &&other)      = delete;
    ~NetworkEventCouplingClient();

    //! Configures outgoing event filtering.
    /*!
     *  The setting will take effect with the next event coupling that gets installed.
     *
     *  /param configuration
     *  Pass <c>nullptr</c> to disable filtering. 
     */
    void ConfigureOutgoingEventFilter(const std::shared_ptr<EventFilterConfiguration> &configuration);
    //! Establishes - and tries to maintain - a network event coupling via TCP connection to the specified host.
    void Connect(const std::string &hostAndPort);
    //! Switches into disconnected mode, where no network event coupling is installed.
    void Disconnect();

  private:
    void OnTcpConnectionEstablished(int fd) override;
    void OnFailedToEstablishTcpConnection() override;
    void OnNetworkEventCouplingErrorState() override;
    void OnTimer() override;

    void ScheduleReconnect();
    void InstallConnector();
    void InstallCoupling(int fd);
    void UninstallCoupling();

    const std::shared_ptr<EventHub>            hub_;             // Thread-safe.
    const std::shared_ptr<IO::ConnectionIO>    connectionIO_;    // Thread-safe.
    const std::shared_ptr<Core::IoBuffers>     ioBuffers_;       // Thread-safe.
    const std::shared_ptr<K::Core::ThreadPool> threadPool_;      // Thread-safe.
    const std::shared_ptr<K::Core::Timers>     timers_;          // Thread-safe.

    const std::shared_ptr<K::Core::RunLoop>   runLoop_;
    const std::string                         protocolVersion_;
    const IO::KeepAliveParameters             keepAliveParameters_;
    std::string                               hostAndPort_;
    std::shared_ptr<Event>                    connectedEvent_;
    std::shared_ptr<Event>                    disconnectedEvent_;
    std::shared_ptr<EventFilterConfiguration> eventFilterConfiguration_;
    std::unique_ptr<IO::TcpConnector>         connector_;
    std::unique_ptr<NetworkEventCoupling>     coupling_;
    std::unique_ptr<Core::Timer>              timer_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_H_
