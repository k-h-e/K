#include <K/IO/TcpConnection.h>

#include <K/Core/Log.h>
#include <K/IO/NetworkTools.h>

using std::shared_ptr;
using std::string;
using K::IO::ConnectionIO;
using K::IO::NetworkTools;

namespace K {
namespace IO {

TcpConnection::TcpConnection(const string &host, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(host, this), bufferSizeThreshold, connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(uint32_t ip4Address, int port, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(ip4Address, port, this), bufferSizeThreshold, connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(int fd, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(fd, bufferSizeThreshold, connectionIO) {
    // Nop.
}

}    // Namesapce IO.
}    // Namespace K.
