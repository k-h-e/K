#include <K/Events/Framework/NetworkEventCouplingServer.h>

#include <K/Core/Log.h>
#include <K/IO/Framework/TcpConnection.h>

using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::string;
using std::to_string;
using std::chrono::milliseconds;
using K::Core::Log;
using K::Core::ThreadPool;
using K::Core::Timers;
using K::Core::Framework::RunLoop;
using K::Core::Framework::Timer;
using K::IO::ConnectionIO;
using K::IO::KeepAliveParameters;
using K::IO::Framework::ListenSocket;
using K::IO::Framework::TcpConnection;

namespace K {
namespace Events {
namespace Framework {

NetworkEventCouplingServer::NetworkEventCouplingServer(
            int port, const string &protocolVersion, const KeepAliveParameters &keepAliveParameters,
            const shared_ptr<EventHub> &hub, const shared_ptr<RunLoop> &runLoop,
            const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<Timers> &timers,
            const shared_ptr<ThreadPool> &threadPool)
        : hub_{hub},
          connectionIO_{connectionIO},
          timers_{timers},
          threadPool_{threadPool},
          runLoop_{runLoop},
          handler_{nullptr},
          handlerActivationId_{0},
          port_{port},
          protocolVersion_{protocolVersion},
          keepAliveParameters_{keepAliveParameters},
          signalCouplingInstallation_{false} {
    runLoopClientId_ = runLoop_->AddClient(this);
    InstallListenSocket();
}

NetworkEventCouplingServer::~NetworkEventCouplingServer() {
    UninstallCoupling();
    UninstallTimer();
    UninstallListenSocket();

    runLoop_->RemoveClient(runLoopClientId_);

    // TODO: Check deconstruction.
}

void NetworkEventCouplingServer::Register(HandlerInterface *handler, int id) {
    handler_             = handler;
    handlerActivationId_ = handler ? id : 0;

    if (handler_) {
        if (coupling_) {
            signalCouplingInstallation_ = true;
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
    } else {
        signalCouplingInstallation_ = false;
    }
}

void NetworkEventCouplingServer::Activate(bool deepActivation) {
    (void)deepActivation;
    if (signalCouplingInstallation_) {
        if (handler_) {
            handler_->OnNetworkEventCouplingInstalled(handlerActivationId_);
        }
        signalCouplingInstallation_ = false;
    }
}

void NetworkEventCouplingServer::OnListenSocketAcceptedConnection(int id, unique_ptr<TcpConnection> connection) {
    (void)id;
    InstallCoupling(std::move(connection));

    if (handler_) {
        handler_->OnNetworkEventCouplingInstalled(handlerActivationId_);
    }
}

void NetworkEventCouplingServer::OnListenSocketErrorState(int id) {
    (void)id;
    UninstallListenSocket();
    InstallTimer();
}

void NetworkEventCouplingServer::OnNetworkEventCouplingErrorState(int id) {
    (void)id;
    UninstallCoupling();

    if (handler_) {
        handler_->OnNetworkEventCouplingUninstalled(handlerActivationId_);
    }
}

void NetworkEventCouplingServer::OnTimer(int id) {
    (void)id;
    InstallListenSocket();
    UninstallTimer();
}

void NetworkEventCouplingServer::InstallListenSocket() {
    UninstallListenSocket();
    listenSocket_ = make_unique<ListenSocket>(port_, runLoop_, connectionIO_, threadPool_);
    listenSocket_->Register(this, 0);
    Log::Print(Log::Level::Debug, this, [&]{ return "listen socket installed, port=" + to_string(port_); });
}

void NetworkEventCouplingServer::UninstallListenSocket() {
    if (listenSocket_) {
        listenSocket_->Register(nullptr, 0);    // Defensive.
        listenSocket_.reset();
        Log::Print(Log::Level::Debug, this, [&]{ return "listen socket uninstalled, port=" + to_string(port_); });
    }
}

void NetworkEventCouplingServer::InstallCoupling(unique_ptr<TcpConnection> connection) {
    UninstallCoupling();
    coupling_ = make_unique<NetworkEventCoupling>(std::move(connection), protocolVersion_, keepAliveParameters_, hub_,
                                                  runLoop_, timers_);
    coupling_->Register(this, 0);
    Log::Print(Log::Level::Debug, this, [&]{ return "network event coupling installed, port=" + to_string(port_); });
}

void NetworkEventCouplingServer::UninstallCoupling() {
    if (coupling_) {
        coupling_->Register(nullptr, 0);    // Defensive.
        coupling_.reset();
        Log::Print(Log::Level::Debug, this, [&]{
            return "network event coupling uninstalled, port=" + to_string(port_);
        });
    }
}

void NetworkEventCouplingServer::InstallTimer() {
    UninstallTimer();
    timer_ = make_unique<Timer>(milliseconds(3000), runLoop_, timers_);
    timer_->Register(this, 0);
}

void NetworkEventCouplingServer::UninstallTimer() {
    if (timer_) {
        timer_->Register(nullptr, 0);    // Defensive.
        timer_.reset();
    }
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

