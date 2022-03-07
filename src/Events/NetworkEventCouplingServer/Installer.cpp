#include "Installer.h"

#include <K/Core/Log.h>
#include <K/IO/ListenSocket.h>
#include "SharedState.h"

using std::make_shared;
using std::mutex;
using std::shared_ptr;
using std::unique_lock;
using K::Core::Log;
using K::Core::ThreadPool;
using K::IO::ConnectionIO;
using K::IO::ListenSocket;

namespace K {
namespace Events {

NetworkEventCouplingServer::Installer::Installer(
    int port, const shared_ptr<SharedState> &shared, const shared_ptr<ConnectionIO> &connectionIO,
    const shared_ptr<ThreadPool> &threadPool)
        : shared_(shared),
          connectionIO_(connectionIO),
          threadPool_(threadPool),
          port_(port),
          installationStep_(0) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    InstallListenSocket(critical);
}    // ......................................................................................... critical section, end.

NetworkEventCouplingServer::Installer::~Installer() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    UninstallListenSocket(critical);
}    // ......................................................................................... critical section, end.

bool NetworkEventCouplingServer::Installer::OnTimer(int timer) {
    (void)timer;
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    ++installationStep_;
    switch (installationStep_) {
        case 1:
            UninstallListenSocket(critical);
            return true;
        case 2:
        default:
            InstallListenSocket(critical);
            installationStep_ = 0;
            return false;
    }
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingServer::Installer::InstallListenSocket(unique_lock<mutex> &critical) {
    UninstallListenSocket(critical);
    Log::Print(Log::Level::Debug, this, [&]{ return "installing listen socket"; });
    listenSocket_ = make_shared<ListenSocket>(port_, connectionIO_, threadPool_);
    listenSocket_->Register(shared_.get(), false);
}

void NetworkEventCouplingServer::Installer::UninstallListenSocket(unique_lock<mutex> &critical) {
    (void)critical;
    if (listenSocket_) {
        Log::Print(Log::Level::Debug, this, [&]{ return "uninstalling listen socket"; });
        listenSocket_->Register(nullptr, false);
        listenSocket_.reset();
    }
}

}    // Namespace Events.
}    // Namespace K.

