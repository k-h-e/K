#ifndef K_EVENTS_FRAMEWORK_EVENTNOTIFIER_LOOPTHREADSTATE_H_
#define K_EVENTS_FRAMEWORK_EVENTNOTIFIER_LOOPTHREADSTATE_H_

#include <K/Core/Framework/RunLoop.h>
#include <K/Events/Framework/EventNotifier.h>

namespace K {
namespace Events {
namespace Framework {

//! Notifier state exclusively accessed from the run loop thread.
struct EventNotifier::LoopThreadState : public virtual Core::Framework::RunLoop::ClientInterface {
    const std::shared_ptr<EventHub>                 hub;
    int                                             clientLoopId;
    const std::shared_ptr<Core::Framework::RunLoop> runLoop;
    int                                             runLoopClientId;
    HandlerInterface                                *handler;
    int                                             handlerAssociatedId;

    LoopThreadState(const std::shared_ptr<EventHub> &hub, int clientLoopId,
                    const std::shared_ptr<Core::Framework::RunLoop> &runLoop);
    LoopThreadState(const LoopThreadState &other)            = delete;
    LoopThreadState &operator=(const LoopThreadState &other) = delete;
    LoopThreadState(LoopThreadState &&other)                 = delete;
    LoopThreadState &operator=(LoopThreadState &&other)      = delete;
    ~LoopThreadState()                                       = default;

    void Activate(bool deepActivation) override;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_EVENTNOTIFIER_LOOPTHREADSTATE_H_
