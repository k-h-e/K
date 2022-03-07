#ifndef K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_
#define K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_

#include <memory>
#include <K/IO/Framework/ListenSocket.h>
#include <K/Events/Framework/NetworkEventCoupling.h>

namespace K {
    namespace Core {
        class ThreadPool;
        namespace Framework {
            class RunLoop;
        }
    }
    namespace IO {
        class ConnectionIO;
    }
    namespace Events {
        class EventLoopHub;
    }
}

namespace K {
namespace Events {
//! Framework- (<c>Core::Framework::RunLoop</c>-) enabled variants of event mechanism classes.
namespace Framework {

//! Installs network event couplings for incoming network connections.
class NetworkEventCouplingServer : public virtual IO::Framework::ListenSocket::HandlerInterface,
                                   public virtual Events::Framework::NetworkEventCoupling::HandlerInterface {
  public:
    NetworkEventCouplingServer(
        int port, const std::string &protocolVersion, const std::shared_ptr<EventLoopHub> &hub,
        const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO, const std::shared_ptr<Core::ThreadPool> &threadPool);
    NetworkEventCouplingServer()                                                   = delete;
    NetworkEventCouplingServer(const NetworkEventCouplingServer &other)            = delete;
    NetworkEventCouplingServer &operator=(const NetworkEventCouplingServer &other) = delete;
    NetworkEventCouplingServer(NetworkEventCouplingServer &&other)                 = delete;
    NetworkEventCouplingServer &operator=(NetworkEventCouplingServer &&other)      = delete;
    ~NetworkEventCouplingServer();

    void OnListenSocketAcceptedConnection(int id, std::unique_ptr<IO::Framework::TcpConnection> connection)
        override;
    void OnListenSocketErrorState(int id) override;
    void OnNetworkEventCouplingErrorState(int id) override;

  private:
   void InstallListenSocket();

   const std::shared_ptr<EventLoopHub>     hub_;             // Thread safe.
   const std::shared_ptr<IO::ConnectionIO> connectionIO_;    // Thread safe.
   const std::shared_ptr<Core::ThreadPool> threadPool_;      // Thread safe.

   const std::shared_ptr<Core::Framework::RunLoop> runLoop_;
   int                                             port_;
   std::string                                     protocolVersion_;
   std::unique_ptr<IO::Framework::ListenSocket>    listenSocket_;
   std::unique_ptr<NetworkEventCoupling>           coupling_;

};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_H_
