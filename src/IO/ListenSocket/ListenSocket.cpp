#include <K/IO/ListenSocket.h>

#include "SharedState.h"

using std::make_shared;
using std::shared_ptr;
using K::Core::ThreadPool;
using K::IO::Socket;
using K::IO::TcpConnection;

namespace K {
namespace IO {

ListenSocket::ListenSocket(int port, const shared_ptr<ConnectionIO> &connectionIO,
                           const shared_ptr<ThreadPool> &threadPool) {
    shared_ = make_shared<SharedState>(port, connectionIO, threadPool);
}

shared_ptr<Socket> ListenSocket::Accept() {
    return shared_->Accept();
}

shared_ptr<TcpConnection> ListenSocket::AcceptConnection() {
    return shared_->AcceptConnection();
}

void ListenSocket::ShutDown() {
    shared_->ShutDown();
}

bool ListenSocket::ErrorState() {
    return shared_->ErrorState();
}

}    // Namesapce IO.
}    // Namespace K.
