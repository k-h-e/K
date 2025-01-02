/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_LISTENSOCKET_LOOPTHREADSTATE_H_
#define K_IO_LISTENSOCKET_LOOPTHREADSTATE_H_

#include <deque>
#include <K/Core/RunLoop.h>
#include <K/IO/ListenSocket.h>
#include <K/IO/Deprecated/ListenSocket.h>

namespace K {
    namespace IO {
        class TcpConnection;
    }
}

namespace K {
namespace IO {

//! State exclusively accessed from the run loop thread.
struct ListenSocket::LoopThreadState : public virtual Core::RunLoop::ClientInterface {
    const std::shared_ptr<SynchronizedState>        synchronizedState;    // Thread-safe.
    const std::shared_ptr<Deprecated::ListenSocket> listenSocket;         // Thread-safe.
    const std::shared_ptr<IO::ConnectionIO>         connectionIO;         // Thread-safe.

    const std::shared_ptr<Core::RunLoop>       runLoop;
    int                                        runLoopClientId;

    HandlerInterface                           *handler;
    std::deque<std::unique_ptr<TcpConnection>> acceptedConnections;
    bool                                       error;
    bool                                       newHandlerRegistered;
    bool                                       activationRequested;
    bool                                       requestedActivationIsDeep;

    LoopThreadState(
        const std::shared_ptr<Core::RunLoop> &aRunLoop, const std::shared_ptr<SynchronizedState> &aSynchronizedState,
        const std::shared_ptr<Deprecated::ListenSocket> &aListenSocket,
        const std::shared_ptr<IO::ConnectionIO> &aConnectionIO);
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

#endif    // K_IO_LISTENSOCKET_LOOPTHREADSTATE_H_
