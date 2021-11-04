#include <K/IO/TcpConnection.h>

#include <K/Core/Log.h>
#include <K/IO/NetworkTools.h>

using std::shared_ptr;
using std::string;
using K::Core::Result;
using K::IO::ConnectionIO;
using K::IO::NetworkTools;

namespace K {
namespace IO {

TcpConnection::TcpConnection(const string &hostAndPort, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(hostAndPort, nullptr), bufferSizeThreshold, nullptr,
                             connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(const string &hostAndPort, const shared_ptr<Result> &resultAcceptor,
                             const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(hostAndPort, nullptr), bufferSizeThreshold, resultAcceptor,
                             connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(const string &host, int port, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(host, port, nullptr), bufferSizeThreshold, nullptr,
                             connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(const string &host, int port, const shared_ptr<Result> &resultAcceptor,
                             const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(host, port, nullptr), bufferSizeThreshold, resultAcceptor,
                             connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(uint32_t ip4Address, int port, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(ip4Address, port, nullptr), bufferSizeThreshold, nullptr,
                             connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(uint32_t ip4Address, int port, const shared_ptr<Result> &resultAcceptor,
                             const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(ip4Address, port, nullptr), bufferSizeThreshold, resultAcceptor,
                             connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(int fd, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(fd, bufferSizeThreshold, nullptr, connectionIO) {
    // Nop.
}

}    // Namesapce IO.
}    // Namespace K.
