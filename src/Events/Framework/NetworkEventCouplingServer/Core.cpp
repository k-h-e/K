#include "Core.h"

#include <K/Core/Log.h>
#include <K/Core/Framework/Timer.h>
#include <K/IO/Framework/ListenSocket.h>
#include <K/IO/Framework/TcpConnection.h>
#include <K/Events/Framework/NetworkEventCoupling.h>

using std::make_unique;
using std::move;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::unique_ptr;
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

NetworkEventCouplingServer::Core::Core(
        int port, const string &protocolVersion, const KeepAliveParameters &keepAliveParameters,
        const shared_ptr<EventLoopHub> &hub, const shared_ptr<RunLoop> &runLoop,
        const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<Timers> &timers,
        const shared_ptr<ThreadPool> &threadPool)
            : hub_(hub),
              connectionIO_(connectionIO),
              timers_(timers),
              threadPool_(threadPool),
              runLoop_(runLoop),
              port_(port),
              protocolVersion_(protocolVersion),
              keepAliveParameters_(keepAliveParameters) {
    InstallListenSocket();
}

NetworkEventCouplingServer::Core::~Core() {
    UninstallTimer();
    UninstallCoupling();
    UninstallListenSocket();

    // TODO: Check deconstruction.
}

void NetworkEventCouplingServer::Core::OnListenSocketAcceptedConnection(int id, unique_ptr<TcpConnection> connection) {
    (void)id;
    InstallCoupling(move(connection));
}

void NetworkEventCouplingServer::Core::OnListenSocketErrorState(int id) {
    (void)id;
    UninstallListenSocket();
    InstallTimer();
}

void NetworkEventCouplingServer::Core::OnNetworkEventCouplingErrorState(int id) {
    (void)id;
    UninstallCoupling();
}

void NetworkEventCouplingServer::Core::OnTimer(int id) {
    (void)id;
    InstallListenSocket();
    UninstallTimer();
}

void NetworkEventCouplingServer::Core::InstallListenSocket() {
    UninstallListenSocket();
    listenSocket_ = make_unique<ListenSocket>(port_, runLoop_, connectionIO_, threadPool_);
    listenSocket_->Register(this, 0);
    Log::Print(Log::Level::Debug, this, [&]{ return "listen socket installed, port=" + to_string(port_); });
}

void NetworkEventCouplingServer::Core::UninstallListenSocket() {
    if (listenSocket_) {
        listenSocket_->Register(nullptr, 0);    // Defensive.
        listenSocket_.reset();
        Log::Print(Log::Level::Debug, this, [&]{ return "listen socket uninstalled, port=" + to_string(port_); });
    }
}

void NetworkEventCouplingServer::Core::InstallCoupling(unique_ptr<TcpConnection> connection) {
    UninstallCoupling();
    coupling_ = make_unique<NetworkEventCoupling>(move(connection), protocolVersion_, keepAliveParameters_, hub_,
                                                  runLoop_, timers_);
    coupling_->Register(this, 0);
    Log::Print(Log::Level::Debug, this, [&]{ return "network event coupling installed, port=" + to_string(port_); });
}

void NetworkEventCouplingServer::Core::UninstallCoupling() {
    if (coupling_) {
        coupling_->Register(nullptr, 0);    // Defensive.
        coupling_.reset();
        Log::Print(Log::Level::Debug, this, [&]{
            return "network event coupling uninstalled, port=" + to_string(port_);
        });
    }
}

void NetworkEventCouplingServer::Core::InstallTimer() {
    UninstallTimer();
    timer_ = make_unique<Timer>(milliseconds(3000), runLoop_, timers_);
    timer_->Register(this, 0);
}

void NetworkEventCouplingServer::Core::UninstallTimer() {
    if (timer_) {
        timer_->Register(nullptr, 0);    // Defensive.
        timer_.reset();
    }
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

