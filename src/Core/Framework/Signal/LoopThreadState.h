////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_FRAMEWORK_SIGNAL_LOOPTHREADSTATE_
#define K_CORE_FRAMEWORK_SIGNAL_LOOPTHREADSTATE_

#include <K/Core/Framework/Signal.h>
#include <K/Core/Framework/RunLoop.h>

namespace K {
namespace Core {
namespace Framework {

class RunLoop;

//! Signal state exclusively accessed from the run loop thread.
struct Signal::LoopThreadState : public virtual Core::Framework::RunLoop::ClientInterface {
    const std::shared_ptr<RunLoop>      runLoop;
    int                                 runLoopClientId;
    HandlerInterface                    *handler;
    int                                 handlerAssociatedId;

    LoopThreadState(const std::shared_ptr<RunLoop> &runLoop);
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

#endif    // K_CORE_FRAMEWORK_SIGNAL_LOOPTHREADSTATE_
