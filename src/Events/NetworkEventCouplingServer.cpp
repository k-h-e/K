/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Events/NetworkEventCouplingServer.h>

#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>

using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::string;
using std::to_string;
using std::chrono::milliseconds;
using K::Core::IoBuffers;
using K::Core::Log;
using K::Core::RunLoop;
using K::Core::ThreadPool;
using K::Core::Timer;
using K::Core::Timers;
using K::IO::ConnectionIO;
using K::IO::KeepAliveParameters;
using K::IO::ListenSocket;
using K::IO::TcpConnection;

namespace K {
namespace Events {

NetworkEventCouplingServer::NetworkEventCouplingServer(
            int port, const string &protocolVersion, const KeepAliveParameters &keepAliveParameters,
            const shared_ptr<EventHub> &hub, const shared_ptr<Event> &connectedEvent,
            const shared_ptr<Event> &disconnectedEvent, const shared_ptr<RunLoop> &runLoop,
            const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<IoBuffers> &ioBuffers,
            const shared_ptr<Timers> &timers, const shared_ptr<ThreadPool> &threadPool)
        : hub_{hub},
          connectionIO_{connectionIO},
          ioBuffers_{ioBuffers},
          timers_{timers},
          threadPool_{threadPool},
          runLoop_{runLoop},
          handler_{nullptr},
          port_{port},
          protocolVersion_{protocolVersion},
          keepAliveParameters_{keepAliveParameters},
          connectedEvent_{connectedEvent},
          disconnectedEvent_{disconnectedEvent},
          signalCouplingInstallation_{false} {
    runLoopClientId_ = runLoop_->AddClient(this);
    InstallListenSocket();
}

NetworkEventCouplingServer::~NetworkEventCouplingServer() {
    UninstallCoupling();
    UninstallTimer();
    UninstallListenSocket();
    runLoop_->RemoveClient(runLoopClientId_);
}

void NetworkEventCouplingServer::Register(HandlerInterface *handler) {
    handler_ = handler;
    if (handler_) {
        if (coupling_) {
            signalCouplingInstallation_ = true;
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
    } else {
        signalCouplingInstallation_ = false;
    }
}

void NetworkEventCouplingServer::ConfigureOutgoingEventFilter(
        const shared_ptr<EventFilterConfiguration> &configuration) {
    eventFilterConfiguration_ = configuration;
}

// ---

void NetworkEventCouplingServer::Activate(bool deepActivation) {
    (void) deepActivation;
    if (signalCouplingInstallation_) {
        if (handler_) {
            handler_->OnNetworkEventCouplingInstalled();
        }
        signalCouplingInstallation_ = false;
    }
}

void NetworkEventCouplingServer::OnListenSocketAcceptedConnection(unique_ptr<TcpConnection> connection) {
    InstallCoupling(std::move(connection));

    if (handler_) {
        handler_->OnNetworkEventCouplingInstalled();
    }
}

void NetworkEventCouplingServer::OnListenSocketErrorState() {
    UninstallListenSocket();
    InstallTimer();
}

void NetworkEventCouplingServer::OnNetworkEventCouplingErrorState() {
    UninstallCoupling();

    if (handler_) {
        handler_->OnNetworkEventCouplingUninstalled();
    }
}

void NetworkEventCouplingServer::OnTimer() {
    InstallListenSocket();
    UninstallTimer();
}

void NetworkEventCouplingServer::InstallListenSocket() {
    UninstallListenSocket();
    listenSocket_ = make_unique<ListenSocket>(port_, runLoop_, connectionIO_, ioBuffers_, threadPool_);
    listenSocket_->Register(this);
    Log::Print(Log::Level::Debug, this, [&]{ return "listen socket installed, port=" + to_string(port_); });
}

void NetworkEventCouplingServer::UninstallListenSocket() {
    if (listenSocket_) {
        listenSocket_->Register(nullptr);    // Defensive.
        listenSocket_.reset();
        Log::Print(Log::Level::Debug, this, [&]{ return "listen socket uninstalled, port=" + to_string(port_); });
    }
}

void NetworkEventCouplingServer::InstallCoupling(unique_ptr<TcpConnection> connection) {
    UninstallCoupling();
    coupling_ = make_unique<NetworkEventCoupling>(std::move(connection), protocolVersion_, keepAliveParameters_, hub_,
                                                  connectedEvent_, disconnectedEvent_, runLoop_, ioBuffers_, timers_);
    coupling_->ConfigureOutgoingEventFilter(eventFilterConfiguration_);
    coupling_->Register(this);
    Log::Print(Log::Level::Debug, this, [&]{ return "network event coupling installed, port=" + to_string(port_); });
}

void NetworkEventCouplingServer::UninstallCoupling() {
    if (coupling_) {
        coupling_->Register(nullptr);    // Defensive.
        coupling_.reset();
        Log::Print(Log::Level::Debug, this, [&]{
            return "network event coupling uninstalled, port=" + to_string(port_);
        });
    }
}

void NetworkEventCouplingServer::InstallTimer() {
    UninstallTimer();
    timer_ = make_unique<Timer>(milliseconds(3000), runLoop_, timers_);
    timer_->Register(this);
}

void NetworkEventCouplingServer::UninstallTimer() {
    if (timer_) {
        timer_->Register(nullptr);    // Defensive.
        timer_.reset();
    }
}

}    // Namespace Events.
}    // Namespace K.

