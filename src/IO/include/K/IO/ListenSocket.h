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
class ListenSocket : public virtual Core::ErrorStateInterface {
  public:
    class HandlerInterface : public virtual Core::Interface {
      public:
        virtual void OnNetworkConnectionAccepted(const std::shared_ptr<TcpConnection> &connection) = 0;
        virtual void OnListenSocketErrorState() = 0;
    };

    ListenSocket(int port, const std::shared_ptr<ConnectionIO> &connectionIO,
                 const std::shared_ptr<Core::ThreadPool> &threadPool);
    ListenSocket(const ListenSocket &other)             = delete;
    ListenSocket &operator=(const ListenSocket &other)  = delete;
    ListenSocket(ListenSocket &&other)                  = delete;
    ListenSocket &operator=(ListenSocket &&other)       = delete;

    //! Registers the specified handler, unregistering any handler potentially registered earlier.
    /*!
     *  The handler methods will get called on an arbitrary thread and must not call back into the <c>ListenSocket</c>.
     *
     *  Pass <c>nullptr</c> to unregister a registered handler. When the method then returns, it is guaranteed that the
     *  handler will not be called again.
     */
    void Register(HandlerInterface *handler);
    bool ErrorState() override;

  private:
    class Acceptor;
    class SharedState;

    std::shared_ptr<SharedState> shared_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_LISTENSOCKET_H_
