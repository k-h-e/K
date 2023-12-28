/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_EVENTNOTIFIER_LOOPTHREADSTATE_H_
#define K_EVENTS_EVENTNOTIFIER_LOOPTHREADSTATE_H_

#include <K/Core/RunLoop.h>
#include <K/Events/EventNotifier.h>

namespace K {
namespace Events {

//! Notifier state exclusively accessed from the run loop thread.
struct EventNotifier::LoopThreadState : public virtual Core::RunLoop::ClientInterface {
    const std::shared_ptr<EventHub>      hub;
    int                                  clientLoopId;
    const std::shared_ptr<Core::RunLoop> runLoop;
    int                                  runLoopClientId;
    HandlerInterface                     *handler;

    LoopThreadState(const std::shared_ptr<EventHub> &hub, int clientLoopId,
                    const std::shared_ptr<Core::RunLoop> &runLoop);
    LoopThreadState(const LoopThreadState &other)            = delete;
    LoopThreadState &operator=(const LoopThreadState &other) = delete;
    LoopThreadState(LoopThreadState &&other)                 = delete;
    LoopThreadState &operator=(LoopThreadState &&other)      = delete;
    ~LoopThreadState()                                       = default;

    void Activate(bool deepActivation) override;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENTNOTIFIER_LOOPTHREADSTATE_H_
