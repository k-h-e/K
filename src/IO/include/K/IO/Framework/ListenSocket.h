////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_FRAMEWORK_LISTENSOCKET_H_
#define K_IO_FRAMEWORK_LISTENSOCKET_H_

#include <memory>
#include <K/Core/ErrorStateInterface.h>

namespace K {
    namespace Core {
        class ThreadPool;
        namespace Framework {
            class RunLoop;
        }
    }
    namespace IO {
        class ConnectionIO;
        class TcpConnection;
    }
}

namespace K {
namespace IO {
namespace Framework {

//! Listen socket, for use with a <c>Core::Framework::RunLoop</c>.
class ListenSocket : public virtual Core::ErrorStateInterface {
  public:
    class HandlerInterface : public virtual Core::Interface {
      public:
        //! Hands over a newly accepted connection to the handler.
        /*!
         *  \param id
         *  ID that was given when the handler was registered.
         */
        virtual void OnListenSocketAcceptedConnection(int id, const std::shared_ptr<TcpConnection> &connection) = 0;
        //! Informs the handler that the listen socket has entered error state.
        /*!
         *  \param id
         *  ID that was given when the handler was registered.
         */
        virtual void OnListenSocketErrorState(int id) = 0;
    };

    ListenSocket(
        int port, const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO, const std::shared_ptr<Core::ThreadPool> &threadPool);
    ListenSocket()                                     = delete;
    ListenSocket(const ListenSocket &other)            = delete;
    ListenSocket &operator=(const ListenSocket &other) = delete;
    ListenSocket(ListenSocket &&other)                 = delete;
    ListenSocket &operator=(ListenSocket &&other)      = delete;
    ~ListenSocket();

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the listen
     *  socket.
     *
     *  The handler is expected to outlive the listen socket.
     *
     *  \param id
     *  ID to be passed along with handler activations for the listen socket. Useful in case one wants to use one
     *  handler with multiple listen sockets.
     */
    void Register(HandlerInterface *handler, int id);
    bool ErrorState() const override;

  private:
    struct LoopThreadState;
    class SynchronizedState;

    std::unique_ptr<LoopThreadState> loopThreadState_;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_LISTENSOCKET_H_
