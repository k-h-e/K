#ifndef K_IO_TCPCONNECTION_H_
#define K_IO_TCPCONNECTION_H_

#include <memory>
#include <K/IO/BufferedConnection.h>

namespace K {
namespace IO {

class ConnectionIO;

//! TCP connection.
class TcpConnection : public BufferedConnection {
  public:
    //! The connection takes ownership over the UNIX file descriptor.
    TcpConnection(int fd, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    TcpConnection(const TcpConnection &other)             = delete;
    TcpConnection &operator=(const TcpConnection &other)  = delete;
    TcpConnection(const TcpConnection &&other)            = delete;
    TcpConnection &operator=(const TcpConnection &&other) = delete;

    //! Establishes a TCP network connection to a host given by name and port, separated by a <c>':'</c>.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    static std::shared_ptr<TcpConnection> ConnectToHost(
        const std::string &host, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO,
        K::Core::Interface *loggingObject);
    //! Establishes a TCP network connection to the specified host.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    static std::shared_ptr<TcpConnection> ConnectToHost(
        uint32_t ip4Address, int port, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO,
        K::Core::Interface *loggingObject);
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_TCPCONNECTION_H_
