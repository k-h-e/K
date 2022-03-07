////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_FRAMEWORK_CONNECTION_LOOPTHREADSTATE_H_
#define K_IO_FRAMEWORK_CONNECTION_LOOPTHREADSTATE_H_

#include <K/Core/RingBuffer.h>
#include <K/Core/Framework/RunLoop.h>
#include <K/IO/Framework/Connection.h>

namespace K {
namespace IO {
namespace Framework {

class NonBlockingIOStreamInterface;

//! Connection state exclusively accessed from the run loop thread.
struct Connection::LoopThreadState : public virtual Core::Framework::RunLoop::ClientInterface {
    const std::shared_ptr<SynchronizedState>        synchronizedState;    // Thread safe.
    const std::shared_ptr<IO::ConnectionIO>         connectionIO;         // Thread safe.

    const std::shared_ptr<Core::Framework::RunLoop> runLoop;
    int                                             runLoopClientId;

    NonBlockingIOStreamInterface::HandlerInterface  *handler;
    int                                             handlerAssociatedId;
    Core::RingBuffer                                readBuffer;
    Core::RingBuffer                                writeBuffer;
    bool                                            readFailed;
    bool                                            writeFailed;
    bool                                            error;
    bool                                            eof;
    bool                                            clientReadPaused;
    bool                                            clientWritePaused;
    bool                                            newHandlerRegistered;
    bool                                            unpauseIORead;
    bool                                            unpauseIOWrite;
    int                                             bufferSizeConstraint;
    bool                                            activationRequested;

    LoopThreadState(
        const std::shared_ptr<Core::Framework::RunLoop> &aRunLoop,
        const std::shared_ptr<SynchronizedState> &aSynchronizedState, int aBufferSizeConstraint,
        const std::shared_ptr<IO::ConnectionIO> &aConnectionIO);
    LoopThreadState()                                        = delete;
    LoopThreadState(const LoopThreadState &other)            = delete;
    LoopThreadState &operator=(const LoopThreadState &other) = delete;
    LoopThreadState(LoopThreadState &&other)                 = delete;
    LoopThreadState &operator=(LoopThreadState &&other)      = delete;
    ~LoopThreadState()                                       = default;

    void RequestActivation();
    void Activate() override;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_CONNECTION_LOOPTHREADSTATE_H_
