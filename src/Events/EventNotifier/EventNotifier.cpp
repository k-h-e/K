/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Events/EventNotifier.h>
#include "Adapter.h"
#include "LoopThreadState.h"

using std::make_unique;
using std::shared_ptr;
using K::Core::RunLoop;
using K::Events::EventHub;

namespace K {
namespace Events {

EventNotifier::EventNotifier(const shared_ptr<EventHub> &hub, int clientLoopId, const shared_ptr<RunLoop> &runLoop)
        : loopThreadState_(make_unique<LoopThreadState>(hub, clientLoopId, runLoop)) {
    loopThreadState_->runLoopClientId = loopThreadState_->runLoop->AddClient(loopThreadState_.get());
    adapter_ = make_unique<Adapter>(loopThreadState_->runLoop, loopThreadState_->runLoopClientId);
    loopThreadState_->hub->RegisterHandler(loopThreadState_->clientLoopId, adapter_.get());
}

EventNotifier::~EventNotifier() {
    loopThreadState_->hub->RegisterHandler(loopThreadState_->clientLoopId, nullptr);
    loopThreadState_->runLoop->RemoveClient(loopThreadState_->runLoopClientId);
}

void EventNotifier::Register(HandlerInterface *handler) {
    if (handler) {
        loopThreadState_->handler = handler;
        loopThreadState_->runLoop->RequestActivation(loopThreadState_->runLoopClientId, false);
    } else  {
        loopThreadState_->handler = nullptr;
    }
}

}    // Namespace Events.
}    // Namespace K.
