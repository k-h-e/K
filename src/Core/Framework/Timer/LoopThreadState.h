////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_FRAMEWORK_TIMER_LOOPTHREADSTATE_
#define K_CORE_FRAMEWORK_TIMER_LOOPTHREADSTATE_

#include <K/Core/Framework/RunLoop.h>
#include <K/Core/Framework/Timer.h>

namespace K {
namespace Core {

class Timers;

namespace Framework {

class RunLoop;

//! Timer state exclusively accessed from the run loop thread.
struct Timer::LoopThreadState : public virtual Core::Framework::RunLoop::ClientInterface {
    const std::shared_ptr<Core::Timers> timers;    // Thread safe.

    const std::shared_ptr<RunLoop>      runLoop;

    int                                 runLoopClientId;
    int                                 timer;
    HandlerInterface                    *handler;
    int                                 handlerAssociatedId;

    LoopThreadState(const std::shared_ptr<RunLoop> &runLoop, const std::shared_ptr<Core::Timers> &timers);
    LoopThreadState(const LoopThreadState &other)            = delete;
    LoopThreadState &operator=(const LoopThreadState &other) = delete;
    LoopThreadState(LoopThreadState &&other)                 = delete;
    LoopThreadState &operator=(LoopThreadState &&other)      = delete;
    ~LoopThreadState()                                       = default;

    void Activate(bool deepActivation) override;
};

}    // Namespace Framework.
}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_FRAMEWORK_TIMER_LOOPTHREADSTATE_
