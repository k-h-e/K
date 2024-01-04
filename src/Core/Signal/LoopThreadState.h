/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SIGNAL_LOOPTHREADSTATE_
#define K_CORE_SIGNAL_LOOPTHREADSTATE_

#include <K/Core/Signal.h>
#include <K/Core/RunLoop.h>

namespace K {
    namespace Core {
        class RunLoop;
    }
}

namespace K {
namespace Core {

//! Signal state exclusively accessed from the run loop thread.
struct Signal::LoopThreadState : public virtual RunLoop::ClientInterface {
    const std::shared_ptr<RunLoop>      runLoop;
    int                                 runLoopClientId;
    HandlerInterface                    *handler;

    LoopThreadState(const std::shared_ptr<RunLoop> &runLoop);
    LoopThreadState(const LoopThreadState &other)            = delete;
    LoopThreadState &operator=(const LoopThreadState &other) = delete;
    LoopThreadState(LoopThreadState &&other)                 = delete;
    LoopThreadState &operator=(LoopThreadState &&other)      = delete;
    ~LoopThreadState()                                       = default;

    void Activate(bool deepActivation) override;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SIGNAL_LOOPTHREADSTATE_
