////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_FRAMEWORK_CONNECTION_H_
#define K_IO_FRAMEWORK_CONNECTION_H_

#include <memory>
#include <optional>
#include <K/Core/Framework/NonBlockingIOStreamInterface.h>
#include <K/Core/Framework/RunLoop.h>

namespace K {
    namespace Core {
        class Result;
        namespace Framework {
            class RunLoop;
        }
    }
    namespace IO {
        class ConnectionIO;
    }
}

namespace K {
namespace IO {
//! Framework- (<c>Core::Framework::RunLoop</c>-) enabled variants of I/O classes.
namespace Framework {

//! Connection, for use with a <c>Core::Framework::RunLoop</c>.
class Connection : public virtual Core::Framework::NonBlockingIOStreamInterface {
  public:
    //! Creates a new connection for the specified UNIX file descriptor.
    /*!
     *  \param fd
     *  The connection takes ownership over the UNIX file descriptor. Bad file descriptors (<c>nullopt</c> or negative
     *  ones) are accepted, with the connection entering error state right away.
     *
     *  \param resultAcceptor
     *  The result acceptor is optional.
     */
    Connection(
        std::optional<int> fd, int bufferSizeConstraint, const std::shared_ptr<Core::Result> &resultAcceptor,
        const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO);
    Connection()                                   = delete;
    Connection(const Connection &other)            = delete;
    Connection &operator=(const Connection &other) = delete;
    Connection(Connection &&other)                 = delete;
    Connection &operator=(Connection &&other)      = delete;
    ~Connection();

    void Register(NonBlockingIOStreamInterface::HandlerInterface *handler, int id) override;
    void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) override;
    int ReadNonBlocking(void *buffer, int bufferSize) override;
    bool ReadFailed() const override;
    void ClearReadFailed() override;
    int WriteNonBlocking(const void *data, int dataSize) override;
    bool WriteFailed() const override;
    void ClearWriteFailed() override;
    bool ErrorState() const override;
    bool Eof() const override;

  private:
    struct LoopThreadState;
    class SynchronizedState;

    static int ValidateBufferSizeConstraint(int bufferSizeConstraint);

    std::unique_ptr<LoopThreadState> loopThreadState_;
    std::optional<int>               fd_;
    std::shared_ptr<Core::Result>    finalResultAcceptor_;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_CONNECTION_H_
