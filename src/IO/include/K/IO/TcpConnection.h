#ifndef K_IO_TCPCONNECTION_H_
#define K_IO_TCPCONNECTION_H_

#include <memory>
#include <K/IO/BufferedConnection.h>

namespace K {

namespace Core {
    class Result;
}

namespace IO {

class ConnectionIO;

//! TCP connection.
class TcpConnection : public BufferedConnection {
  public:
    //! Establishes a TCP network connection to the specified host, given by name and port, separated by a <c>':'</c>.
    TcpConnection(const std::string &hostAndPort, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host, given by name and port, separated by a <c>':'</c>.
    TcpConnection(const std::string &hostAndPort, const std::shared_ptr<Core::Result> &resultAcceptor,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host.
    TcpConnection(const std::string &host, int port, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host.
    TcpConnection(const std::string &host, int port, const std::shared_ptr<Core::Result> &resultAcceptor,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host.
    TcpConnection(uint32_t ip4Address, int port, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host.
    TcpConnection(uint32_t ip4Address, int port, const std::shared_ptr<Core::Result> &resultAcceptor,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    TcpConnection(int fd, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    TcpConnection(const TcpConnection &other)             = delete;
    TcpConnection &operator=(const TcpConnection &other)  = delete;
    TcpConnection(const TcpConnection &&other)            = delete;
    TcpConnection &operator=(const TcpConnection &&other) = delete;

  private:
    static const int bufferSizeThreshold = 4096;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_TCPCONNECTION_H_
