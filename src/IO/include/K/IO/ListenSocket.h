#ifndef K_IO_LISTENSOCKET_H_
#define K_IO_LISTENSOCKET_H_

#include <memory>
#include <K/Core/ErrorStateInterface.h>

namespace K {

namespace Core {
    class ThreadPool;
}

namespace IO {

class Socket;
class TcpConnection;
class ConnectionIO;

//! Listen socket.
/*!
 * Thread-safe (i.e. all public methods).
 */
class ListenSocket : public virtual K::Core::ErrorStateInterface {
  public:
    ListenSocket(int port, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO,
                 const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    ListenSocket(const ListenSocket &other)             = delete;
    ListenSocket &operator=(const ListenSocket &other)  = delete;
    ListenSocket(ListenSocket &&other)                  = delete;
    ListenSocket &operator=(ListenSocket &&other)       = delete;

    //! Accepts a new connection from the listen socket.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    std::shared_ptr<Socket> Accept();
    //! Accepts a new connection from the listen socket.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    std::shared_ptr<TcpConnection> AcceptConnection();
    //! Shuts down the listen socket, causing active <c>Accept()</c> calls to return and fail.
    void ShutDown();
    bool ErrorState() override;

  private:
    class Acceptor;
    class SharedState;

    std::shared_ptr<SharedState> shared_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_LISTENSOCKET_H_
