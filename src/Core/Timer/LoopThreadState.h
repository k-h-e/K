/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_TIMER_LOOPTHREADSTATE_
#define K_CORE_TIMER_LOOPTHREADSTATE_

#include <K/Core/RunLoop.h>
#include <K/Core/Timer.h>

namespace K {
    namespace Core {
        class RunLoop;
        class Timers;
    }
}

namespace K {
namespace Core {

//! Timer state exclusively accessed from the run loop thread.
struct Timer::LoopThreadState : public virtual RunLoop::ClientInterface {
    const std::shared_ptr<Core::Timers> timers;    // Thread safe.

    const std::shared_ptr<RunLoop>      runLoop;

    int                                 runLoopClientId;
    int                                 timer;
    HandlerInterface                    *handler;

    LoopThreadState(const std::shared_ptr<RunLoop> &runLoop, const std::shared_ptr<Core::Timers> &timers);
    LoopThreadState(const LoopThreadState &other)            = delete;
    LoopThreadState &operator=(const LoopThreadState &other) = delete;
    LoopThreadState(LoopThreadState &&other)                 = delete;
    LoopThreadState &operator=(LoopThreadState &&other)      = delete;
    ~LoopThreadState()                                       = default;

    void Activate(bool deepActivation) override;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TIMER_LOOPTHREADSTATE_
