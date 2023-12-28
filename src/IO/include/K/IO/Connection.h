/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_CONNECTION_H_
#define K_IO_CONNECTION_H_

#include <memory>
#include <optional>
#include <K/Core/NonBlockingIOStreamInterface.h>
#include <K/Core/RunLoop.h>

namespace K {
    namespace Core {
        class ResultAcceptor;
        class RunLoop;
    }
    namespace IO {
        class ConnectionIO;
    }
}

namespace K {
namespace IO {

//! <c>RunLoop</c>-enabled connection.
class Connection : public virtual Core::NonBlockingIOStreamInterface {
  public:
    //! Interface to connection handlers.
    class HandlerInterface : public virtual K::Core::Interface {
    public:
        //! Tells the handler that the connection has just become ready to be read from.
        virtual void OnStreamReadyRead() = 0;
        //! Tells the handler that the connection has just become ready to be written to.
        virtual void OnStreamReadyWrite() = 0;
    };

    //! Creates a new connection for the specified UNIX file descriptor.
    /*!
     *  \param fd
     *  The connection takes ownership over the UNIX file descriptor. Bad file descriptors (<c>nullopt</c> or negative
     *  ones) are accepted, with the connection entering error state right away.
     */
    Connection(
        std::optional<int> fd, int bufferSizeConstraint, const std::shared_ptr<Core::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO);
    Connection()                                   = delete;
    Connection(const Connection &other)            = delete;
    Connection &operator=(const Connection &other) = delete;
    Connection(Connection &&other)                 = delete;
    Connection &operator=(Connection &&other)      = delete;
    ~Connection();

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the connection.
     *
     *  The handler is expected to outlive the connection.
     */
    void Register(HandlerInterface *handler);

    void SetCloseResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) override;
    int ReadNonBlocking(void *buffer, int bufferSize) override;
    int WriteNonBlocking(const void *data, int dataSize) override;
    bool ErrorState() const override;
    std::optional<Error> StreamError() const override;

  private:
    struct LoopThreadState;
    class SynchronizedState;

    static int ValidateBufferSizeConstraint(int bufferSizeConstraint);

    std::unique_ptr<LoopThreadState>      loopThreadState_;
    std::optional<int>                    fd_;
    std::shared_ptr<Core::ResultAcceptor> closeResultAcceptor_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONNECTION_H_
