#include <K/Events/Framework/NetworkEventCouplingClient.h>

#include <K/Core/Log.h>
#include <K/IO/Framework/TcpConnection.h>
#include <K/Events/Framework/NetworkEventCoupling.h>

using std::make_unique;
using std::shared_ptr;
using std::string;
using std::chrono::milliseconds;
using K::Core::Log;
using K::Core::ThreadPool;
using K::Core::Timers;
using K::Core::Framework::RunLoop;
using K::Core::Framework::Timer;
using K::IO::ConnectionIO;
using K::IO::KeepAliveParameters;
using K::IO::Framework::TcpConnection;
using K::IO::Framework::TcpConnector;

namespace K {
namespace Events {
namespace Framework {

NetworkEventCouplingClient::NetworkEventCouplingClient(
    const string &protocolVersion, const KeepAliveParameters &keepAliveParameters, const shared_ptr<EventHub> &hub,
    const shared_ptr<RunLoop> &runLoop, const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<Timers> &timers,
    const shared_ptr<ThreadPool> &threadPool)
        : hub_{hub},
          connectionIO_{connectionIO},
          threadPool_{threadPool},
          timers_{timers},
          runLoop_{runLoop},
          protocolVersion_{protocolVersion},
          keepAliveParameters_{keepAliveParameters} {
    // Nop.

};

NetworkEventCouplingClient::~NetworkEventCouplingClient() {
    Disconnect();
}

void NetworkEventCouplingClient::Connect(const string &hostAndPort) {
    Disconnect();
    hostAndPort_ = hostAndPort;
    Log::Print(Log::Level::Debug, this, [&]{ return "connecting to " + hostAndPort_ + "..."; });
    InstallConnector();
}

void NetworkEventCouplingClient::Disconnect() {
    connector_.reset();
    timer_.reset();
    UninstallCoupling();
    hostAndPort_.clear();
    Log::Print(Log::Level::Debug, this, [&]{ return "disconnected"; });
}

void NetworkEventCouplingClient::OnTcpConnectionEstablished(int id, int fd) {
    (void)id;
    connector_.reset();
    InstallCoupling(fd);
}

void NetworkEventCouplingClient::OnFailedToEstablishTcpConnection(int id) {
    (void)id;
    Log::Print(Log::Level::Warning, this, [&]{
        return "failed to connect to " + hostAndPort_ + ", scheduling reconnect...";
    });
    connector_.reset();
    ScheduleReconnect();
}

void NetworkEventCouplingClient::OnNetworkEventCouplingErrorState(int id) {
    (void)id;
    UninstallCoupling();
    Log::Print(Log::Level::Warning, this, [&]{
        return "TCP connection down, host=" + hostAndPort_ + ". Scheduling reconnect...";
    });
    ScheduleReconnect();
}

void NetworkEventCouplingClient::OnTimer(int id) {
    (void)id;
    timer_.reset();
    Log::Print(Log::Level::Debug, this, [&]{ return "attempting to reconnect to " + hostAndPort_ + "..."; });
    InstallConnector();
}

void NetworkEventCouplingClient::ScheduleReconnect() {
    timer_ = make_unique<Timer>(milliseconds(6000), runLoop_, timers_);
    timer_->Register(this, 0);
}

void NetworkEventCouplingClient::InstallConnector() {
    connector_ = make_unique<TcpConnector>(hostAndPort_, static_cast<IO::TcpConnector::HandlerInterface *>(this), 0,
                                           runLoop_, threadPool_);
}

void NetworkEventCouplingClient::InstallCoupling(int fd) {
    auto connection = make_unique<TcpConnection>(fd, runLoop_, connectionIO_);
    coupling_ = make_unique<NetworkEventCoupling>(std::move(connection), protocolVersion_, keepAliveParameters_, hub_,
                                                  runLoop_, timers_);
    coupling_->Register(this, 0);
    Log::Print(Log::Level::Debug, this, [&]{ return "network event coupling installed, host=" + hostAndPort_; });
}

void NetworkEventCouplingClient::UninstallCoupling() {
    if (coupling_) {
        coupling_.reset();
        Log::Print(Log::Level::Debug, this, [&]{
            return "network event coupling uninstalled, host=" + hostAndPort_;
        });
    }
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.
