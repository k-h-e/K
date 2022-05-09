#ifndef K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_
#define K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_

#include <memory>
#include <string>
#include <K/Core/Framework/Timer.h>
#include <K/IO/KeepAliveParameters.h>
#include <K/IO/Framework/ListenSocket.h>
#include <K/Events/Framework/NetworkEventCoupling.h>

namespace K {
    namespace Core {
        class ThreadPool;
        class Timers;
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
//! Framework- (<c>K::Core::Framework::RunLoop</c>-) enabled variants of event mechanism classes.
namespace Framework {

//! Installs network event couplings for incoming network connections.
class NetworkEventCouplingServer : public virtual K::Core::Interface,
                                   private virtual IO::Framework::ListenSocket::HandlerInterface,
                                   private virtual Events::Framework::NetworkEventCoupling::HandlerInterface,
                                   private virtual K::Core::Framework::Timer::HandlerInterface {
  public:
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

  private:
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
    int                                                port_;
    std::string                                        protocolVersion_;
    IO::KeepAliveParameters                            keepAliveParameters_;
    std::unique_ptr<IO::Framework::ListenSocket>       listenSocket_;
    std::unique_ptr<NetworkEventCoupling>              coupling_;
    std::unique_ptr<K::Core::Framework::Timer>         timer_;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_
