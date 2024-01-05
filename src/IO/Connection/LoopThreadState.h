/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_CONNECTION_LOOPTHREADSTATE_H_
#define K_IO_CONNECTION_LOOPTHREADSTATE_H_

#include <K/Core/RingBuffer.h>
#include <K/Core/RunLoop.h>
#include <K/IO/Connection.h>

namespace K {
namespace IO {

class NonBlockingIOStreamInterface;

//! Connection state exclusively accessed from the run loop thread.
struct Connection::LoopThreadState : public virtual Core::RunLoop::ClientInterface {
    const std::shared_ptr<SynchronizedState> synchronizedState;    // Thread safe.
    const std::shared_ptr<IO::ConnectionIO>  connectionIO;         // Thread safe.

    const std::shared_ptr<Core::RunLoop>     runLoop;
    int                                      runLoopClientId;

    Connection::HandlerInterface             *handler;
    Core::RingBuffer                         readBuffer;
    Core::RingBuffer                         writeBuffer;
    std::optional<Error>                     error;
    bool                                     eof;
    bool                                     clientReadPaused;
    bool                                     clientWritePaused;
    bool                                     handlerNeedsReadyRead;
    bool                                     handlerNeedsReadyWrite;
    bool                                     unpauseIORead;
    bool                                     unpauseIOWrite;
    bool                                     readIsNext;
    int                                      bufferSizeConstraint;
    bool                                     activationRequested;
    bool                                     requestedActivationIsDeep;

    LoopThreadState(
        const std::shared_ptr<Core::RunLoop> &aRunLoop, const std::shared_ptr<SynchronizedState> &aSynchronizedState,
        int aBufferSizeConstraint, const std::shared_ptr<IO::ConnectionIO> &aConnectionIO);
    LoopThreadState()                                        = delete;
    LoopThreadState(const LoopThreadState &other)            = delete;
    LoopThreadState &operator=(const LoopThreadState &other) = delete;
    LoopThreadState(LoopThreadState &&other)                 = delete;
    LoopThreadState &operator=(LoopThreadState &&other)      = delete;
    ~LoopThreadState()                                       = default;

    void RequestActivation(bool deepActivation);
    void Activate(bool deepActivation) override;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONNECTION_LOOPTHREADSTATE_H_
