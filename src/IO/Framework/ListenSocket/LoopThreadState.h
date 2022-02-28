////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_FRAMEWORK_LISTENSOCKET_LOOPTHREADSTATE_H_
#define K_IO_FRAMEWORK_LISTENSOCKET_LOOPTHREADSTATE_H_

#include <K/Core/Framework/RunLoop.h>
#include <K/IO/Framework/ListenSocket.h>

namespace K {
    namespace IO {
        class ListenSocket;
    }
}

namespace K {
namespace IO {
namespace Framework {

//! State exclusively accessed from the run loop thread.
struct ListenSocket::LoopThreadState : public virtual Core::Framework::RunLoop::ClientInterface {
    const std::shared_ptr<SynchronizedState>        synchronizedState;    // Thread safe.
    const std::shared_ptr<IO::ListenSocket>         listenSocket;         // Thread safe.

    const std::shared_ptr<Core::Framework::RunLoop> runLoop;
    int                                             runLoopClientId;

    HandlerInterface                                *handler;
    int                                             handlerAssociatedId;
    bool                                            error;
    bool                                            newHandlerRegistered;
    bool                                            activationRequested;

    LoopThreadState(
        const std::shared_ptr<Core::Framework::RunLoop> &aRunLoop,
        const std::shared_ptr<SynchronizedState> &aSynchronizedState,
        const std::shared_ptr<IO::ListenSocket> &aListenSocket);
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

#endif    // K_IO_FRAMEWORK_LISTENSOCKET_LOOPTHREADSTATE_H_
