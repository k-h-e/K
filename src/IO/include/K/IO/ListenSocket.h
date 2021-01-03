#ifndef K_IO_LISTENSOCKET_H_
#define K_IO_LISTENSOCKET_H_

#include <memory>
#include <mutex>
#include <K/Core/ErrorStateInterface.h>

namespace K {
namespace IO {

class SocketStream;
class TcpConnection;
class ConnectionIO;

//! Listen socket.
/*!
 * Thread-safe (i.e. all public methods).
 */
class ListenSocket : public virtual K::Core::ErrorStateInterface {
  public:
    ListenSocket(int port, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    ListenSocket(const ListenSocket &other)             = delete;
    ListenSocket &operator=(const ListenSocket &other)  = delete;
    ListenSocket(const ListenSocket &&other)            = delete;
    ListenSocket &operator=(const ListenSocket &&other) = delete;
    ~ListenSocket();

    //! Accepts a new connection from the listen socket.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    std::shared_ptr<SocketStream> Accept();
    //! Accepts a new connection from the listen socket.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    std::shared_ptr<TcpConnection> AcceptConnection();
    //! Shuts down the listen socket, causing active <c>Accept()</c> calls to return and fail.
    void ShutDown();
    bool ErrorState();

  private:
    int DoAccept();
    void Close();

    std::shared_ptr<K::IO::ConnectionIO> connectionIO_;

    std::mutex lock_;
    int        fd_;
    bool       socketDown_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_LISTENSOCKET_H_
