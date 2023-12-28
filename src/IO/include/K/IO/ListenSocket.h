/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_LISTENSOCKET_H_
#define K_IO_LISTENSOCKET_H_

#include <memory>
#include <K/Core/ErrorStateInterface.h>

namespace K {
    namespace Core {
        class ThreadPool;
        class RunLoop;
    }
    namespace IO {
        class ConnectionIO;
        class TcpConnection;
    }
}

namespace K {
namespace IO {

//! Listen socket, for use with a <c>RunLoop</c>.
class ListenSocket : public virtual Core::ErrorStateInterface {
  public:
    //! Interface to handlers that clients can register with a listen socket.
    class HandlerInterface : public virtual Core::Interface {
      public:
        //! Hands over a newly accepted connection to the handler.
        virtual void OnListenSocketAcceptedConnection(std::unique_ptr<TcpConnection> connection) = 0;
        //! Informs the handler that the listen socket has entered error state.
        virtual void OnListenSocketErrorState() = 0;
    };

    ListenSocket(
        int port, const std::shared_ptr<Core::RunLoop> &runLoop, const std::shared_ptr<IO::ConnectionIO> &connectionIO,
        const std::shared_ptr<Core::ThreadPool> &threadPool);
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
     */
    void Register(HandlerInterface *handler);
    bool ErrorState() const override;

  private:
    struct LoopThreadState;
    class SynchronizedState;

    std::unique_ptr<LoopThreadState> loopThreadState_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_LISTENSOCKET_H_
