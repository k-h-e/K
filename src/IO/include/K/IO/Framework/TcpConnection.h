#ifndef K_IO_FRAMEWORK_TCPCONNECTION_H_
#define K_IO_FRAMEWORK_TCPCONNECTION_H_

#include <memory>
#include <K/IO/Framework/Connection.h>

namespace K {
    namespace Core {
        class Result;
    }
    namespace IO {
        class ConnectionIO;
    }
}

namespace K {
namespace IO {
namespace Framework {

//! TCP connection for use with a <c>Core::Framework::RunLoop</c>.
class TcpConnection : public Connection {
  public:
    //! Establishes a TCP network connection to the specified host, given by name and port, separated by a <c>':'</c>.
    TcpConnection(const std::string &hostAndPort, const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host, given by name and port, separated by a <c>':'</c>.
    TcpConnection(const std::string &hostAndPort, const std::shared_ptr<Core::Result> &resultAcceptor,
                  const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host.
    TcpConnection(const std::string &host, int port, const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host.
    TcpConnection(const std::string &host, int port, const std::shared_ptr<Core::Result> &resultAcceptor,
                  const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host.
    TcpConnection(uint32_t ip4Address, int port,
                  const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a TCP network connection to the specified host.
    TcpConnection(uint32_t ip4Address, int port, const std::shared_ptr<Core::Result> &resultAcceptor,
                  const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Takes over the already open TCP socket represented by the specified UNIX file descriptor.
    /*!
     *  \param fd
     *  The connection takes ownership over the UNIX file descriptor. Bad file descriptors (<c>nullopt</c> or negative
     *  ones) are accepted, with the connection entering error state right away.
     */
    TcpConnection(int fd, const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
                  const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    TcpConnection()                                       = delete;
    TcpConnection(const TcpConnection &other)             = delete;
    TcpConnection &operator=(const TcpConnection &other)  = delete;
    TcpConnection(const TcpConnection &&other)            = delete;
    TcpConnection &operator=(const TcpConnection &&other) = delete;

  private:
    static const int bufferSizeConstraint = 4096;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_TCPCONNECTION_H_
