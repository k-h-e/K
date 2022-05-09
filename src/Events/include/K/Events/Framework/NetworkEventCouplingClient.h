#ifndef K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGCLIENT_H_
#define K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGCLIENT_H_

#include <memory>
#include <string>
#include <K/Core/Framework/Timer.h>
#include <K/IO/KeepAliveParameters.h>
#include <K/IO/Framework/TcpConnector.h>
#include <K/Events/Framework/NetworkEventCoupling.h>

namespace K {
    namespace Core {
        class Timers;
        class ThreadPool;
        namespace Framework {
            class RunLoop;
        }
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
namespace Framework {

//! Establishes a network connection to a remote event loop mechanism and installs a local network event coupling for
//! it.
/*!
 *  If at some point the network connection breaks down, the client will automatically try to reconnect.
 */
class NetworkEventCouplingClient : public virtual Core::Interface,
                                   private virtual IO::TcpConnector::HandlerInterface,
                                   private virtual NetworkEventCoupling::HandlerInterface,
                                   private virtual Core::Framework::Timer::HandlerInterface {
  public:
    NetworkEventCouplingClient(
        const std::string &protocolVersion, const IO::KeepAliveParameters &keepAliveParameters,
        const std::shared_ptr<EventHub> &hub, const std::shared_ptr<K::Core::Framework::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO, const std::shared_ptr<K::Core::Timers> &timers,
        const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    NetworkEventCouplingClient()                                                   = delete;
    NetworkEventCouplingClient(const NetworkEventCouplingClient &other)            = delete;
    NetworkEventCouplingClient &operator=(const NetworkEventCouplingClient &other) = delete;
    NetworkEventCouplingClient(NetworkEventCouplingClient &&other)                 = delete;
    NetworkEventCouplingClient &operator=(NetworkEventCouplingClient &&other)      = delete;
    ~NetworkEventCouplingClient();

    //! Establishes - and tries to maintain - a network event coupling via TCP connection to the specified host.
    void Connect(const std::string &hostAndPort);
    //! Switches into disconnected mode, where no network event coupling is installed.
    void Disconnect();

  private:
    void OnTcpConnectionEstablished(int id, int fd) override;
    void OnFailedToEstablishTcpConnection(int id) override;
    void OnNetworkEventCouplingErrorState(int id) override;
    void OnTimer(int id) override;

    void ScheduleReconnect();
    void InstallConnector();
    void InstallCoupling(int fd);
    void UninstallCoupling();

    const std::shared_ptr<EventHub>            hub_;             // Thread-safe.
    const std::shared_ptr<IO::ConnectionIO>    connectionIO_;    // Thread-safe.
    const std::shared_ptr<K::Core::ThreadPool> threadPool_;      // Thread-safe.
    const std::shared_ptr<K::Core::Timers>     timers_;          // Thread-safe.

    const std::shared_ptr<K::Core::Framework::RunLoop>       runLoop_;
    const std::string                                        protocolVersion_;
    const IO::KeepAliveParameters                            keepAliveParameters_;
    std::string                                              hostAndPort_;
    std::unique_ptr<IO::Framework::TcpConnector>             connector_;
    std::unique_ptr<Events::Framework::NetworkEventCoupling> coupling_;
    std::unique_ptr<Core::Framework::Timer>                  timer_;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGCLIENT_H_
