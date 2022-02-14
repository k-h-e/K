#ifndef K_EVENTS_NETWORKEVENTCOUPLINGSERVER_INSTALLER_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGSERVER_INSTALLER_H_

#include <mutex>
#include <K/Core/Timers.h>
#include <K/Events/NetworkEventCouplingServer.h>

namespace K {

namespace IO {
    class ListenSocket;
}

namespace Events {

//! Installs the listen socket.
/*!
 *  The class is thread-safe (i.e. all public methods).
 */
class NetworkEventCouplingServer::Installer : public virtual Core::Timers::HandlerInterface {
  public:
    Installer(
        int port, const std::shared_ptr<SharedState> &shared, const std::shared_ptr<IO::ConnectionIO> &connectionIO,
        const std::shared_ptr<Core::ThreadPool> &threadPool);
    Installer()                                  = delete;
    Installer(const Installer &other)            = delete;
    Installer &operator=(const Installer &other) = delete;
    Installer(Installer &&other)                 = delete;
    Installer &operator=(Installer &&other)      = delete;
    ~Installer();

    bool OnTimer(int timer) override;

  private:
    // Expects lock to be held.
    void InstallListenSocket(std::unique_lock<std::mutex> &critical);
    // Expects lock to be held.
    void UninstallListenSocket(std::unique_lock<std::mutex> &critical);

    const std::shared_ptr<SharedState>      shared_;          // Thread safe.
    const std::shared_ptr<IO::ConnectionIO> connectionIO_;    // Thread safe.
    const std::shared_ptr<Core::ThreadPool> threadPool_;      // Thread safe.

    std::mutex                         lock_;                 // Protects everything below...
    int                                port_;
    std::shared_ptr<IO::ListenSocket>  listenSocket_;
    int                                installationStep_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGSERVER_INSTALLER_H_
