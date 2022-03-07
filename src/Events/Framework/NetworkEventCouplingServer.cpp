#include <K/Events/Framework/NetworkEventCouplingServer.h>

#include <K/IO/Framework/ListenSocket.h>
#include <K/IO/Framework/TcpConnection.h>
#include <K/Events/Framework/NetworkEventCoupling.h>

using std::make_unique;
using std::move;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using K::Core::ThreadPool;
using K::Core::Framework::RunLoop;
using K::IO::ConnectionIO;
using K::IO::Framework::ListenSocket;
using K::IO::Framework::TcpConnection;

namespace K {
namespace Events {
namespace Framework {

NetworkEventCouplingServer::NetworkEventCouplingServer(
        int port, const string &protocolVersion, const shared_ptr<EventLoopHub> &hub,
        const shared_ptr<RunLoop> &runLoop, const shared_ptr<ConnectionIO> &connectionIO,
        const shared_ptr<ThreadPool> &threadPool)
            : hub_(hub),
              connectionIO_(connectionIO),
              threadPool_(threadPool),
              runLoop_(runLoop),
              port_(port),
              protocolVersion_(protocolVersion) {
    InstallListenSocket();
}

NetworkEventCouplingServer::~NetworkEventCouplingServer() {
    // TODO: Check deconstruction.
}

void NetworkEventCouplingServer::OnListenSocketAcceptedConnection(int id, unique_ptr<TcpConnection> connection) {
    (void)id;
    if (coupling_) {
        coupling_->Register(nullptr, 0);
    }
    coupling_ = make_unique<NetworkEventCoupling>(move(connection), protocolVersion_, hub_);
    coupling_->Register(this, 0);
}

void NetworkEventCouplingServer::OnListenSocketErrorState(int id) {
    (void)id;
    puts("YYY");
}

void NetworkEventCouplingServer::OnNetworkEventCouplingErrorState(int id) {
    (void)id;
    puts("ZZZ");
}

void NetworkEventCouplingServer::InstallListenSocket() {
    listenSocket_ = make_unique<ListenSocket>(port_, runLoop_, connectionIO_, threadPool_);
    listenSocket_->Register(this, 0);
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

