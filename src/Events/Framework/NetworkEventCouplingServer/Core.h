#ifndef K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_CORE_H_
#define K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_CORE_H_

#include <memory>
#include <string>
#include <K/Core/Framework/Timer.h>
#include <K/IO/KeepAliveParameters.h>
#include <K/IO/Framework/ListenSocket.h>
#include <K/Events/Framework/NetworkEventCoupling.h>
#include <K/Events/Framework/NetworkEventCouplingServer.h>

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
        class EventLoopHub;
    }
}

namespace K {
namespace Events {
namespace Framework {

//! Core of the network evenet coupling server.
class NetworkEventCouplingServer::Core : public virtual IO::Framework::ListenSocket::HandlerInterface,
                                         public virtual Events::Framework::NetworkEventCoupling::HandlerInterface,
                                         public virtual K::Core::Framework::Timer::HandlerInterface {
  public:
    Core(
        int port, const std::string &protocolVersion, const IO::KeepAliveParameters &keepAliveParameters,
        const std::shared_ptr<EventLoopHub> &hub, const std::shared_ptr<K::Core::Framework::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO, const std::shared_ptr<K::Core::Timers> &timers,
        const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    Core()                             = delete;
    Core(const Core &other)            = delete;
    Core &operator=(const Core &other) = delete;
    Core(Core &&other)                 = delete;
    Core &operator=(Core &&other)      = delete;
    ~Core();

    void OnListenSocketAcceptedConnection(int id, std::unique_ptr<IO::Framework::TcpConnection> connection)
        override;
    void OnListenSocketErrorState(int id) override;
    void OnNetworkEventCouplingErrorState(int id) override;
    void OnTimer(int id) override;

  private:
   void InstallListenSocket();
   void UninstallListenSocket();
   void InstallCoupling(std::unique_ptr<IO::Framework::TcpConnection> connection);
   void UninstallCoupling();
   void InstallTimer();
   void UninstallTimer();

   const std::shared_ptr<EventLoopHub>        hub_;             // Thread safe.
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

#endif    // K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLINGSERVER_CORE_H_
