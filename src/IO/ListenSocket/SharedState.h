#ifndef K_IO_LISTENSOCKET_SHAREDSTATE_H_
#define K_IO_LISTENSOCKET_SHAREDSTATE_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/Core/ErrorStateInterface.h>
#include <K/IO/ListenSocket.h>

namespace K {

namespace Core {
    class ThreadPool;
}

namespace IO {

class Socket;
class TcpConnection;
class ConnectionIO;

//! Shared state for the listen socket.
/*!
 * Thread-safe (i.e. all public methods).
 */
class ListenSocket::SharedState : public virtual K::Core::ErrorStateInterface,
                                  public virtual K::Core::CompletionHandlerInterface {
  public:
    SharedState(int port, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO,
                const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    SharedState(const SharedState &other)             = delete;
    SharedState &operator=(const SharedState &other)  = delete;
    SharedState(SharedState &&other)                  = delete;
    SharedState &operator=(SharedState &&other)       = delete;
    ~SharedState();

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
    //! To be called from the acceptor thread to deliver an accepted connection.
    void OnConnectionAccepted(int fd);
    bool ErrorState() override;
    void OnCompletion(int completionId) override;

  private:
    // Expects lock to be held.
    int DoAccept(std::unique_lock<std::mutex> &critical);
    // Expects lock to be held.
    void UpdateAcceptor(std::unique_lock<std::mutex> &critical);

    std::mutex                       lock_;    // Protects everything below...
    std::condition_variable              stateChanged_;
    int                                  port_;
    int                                  fd_;
    int                                  numAcceptRequests_;
    int                                  acceptedConnection_;
    std::unique_ptr<Acceptor>            acceptor_;
    bool                                 acceptorThreadRunning_;
    std::shared_ptr<K::IO::ConnectionIO> connectionIO_;
    std::shared_ptr<K::Core::ThreadPool> threadPool_;
    bool                                 error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_LISTENSOCKET_SHAREDSTATE_H_
