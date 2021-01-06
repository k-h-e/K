#include <K/IO/TcpConnection.h>

#include <K/Core/Log.h>
#include <K/IO/NetworkTools.h>

using std::shared_ptr;
using std::make_shared;
using std::string;
using K::IO::ConnectionIO;
using K::IO::NetworkTools;

namespace K {
namespace IO {

TcpConnection::TcpConnection(int fd, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(fd, 4096, connectionIO) {
    // Nop.
}

shared_ptr<TcpConnection> TcpConnection::ConnectToHost(
        const string &host, const shared_ptr<ConnectionIO> &connectionIO, Core::Interface *loggingObject) {
    int fd = NetworkTools::ConnectTcp(host, loggingObject);
    return (fd >= 0) ? shared_ptr<TcpConnection>(new TcpConnection(fd, connectionIO)) : nullptr;
}

shared_ptr<TcpConnection> TcpConnection::ConnectToHost(
        uint32_t ip4Address, int port, const shared_ptr<ConnectionIO> &connectionIO, Core::Interface *loggingObject) {
    int fd = NetworkTools::ConnectTcp(ip4Address, port, loggingObject);
    return (fd >= 0) ? shared_ptr<TcpConnection>(new TcpConnection(fd, connectionIO)) : nullptr;
}

}    // Namesapce IO.
}    // Namespace K.
