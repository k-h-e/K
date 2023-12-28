/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "LoopThreadState.h"

using std::shared_ptr;
using K::Core::RunLoop;

namespace K {
namespace Events {

EventNotifier::LoopThreadState::LoopThreadState(const shared_ptr<EventHub> &hub, int clientLoopId,
                                                const shared_ptr<RunLoop> &runLoop)
        : hub(hub),
          clientLoopId(clientLoopId),
          runLoop(runLoop),
          runLoopClientId(0),
          handler(nullptr) {
    // Nop.
}

void EventNotifier::LoopThreadState::Activate(bool deepActivation) {
    (void) deepActivation;
    if (handler) {
        handler->OnEventsAvailable();
    }
}

}    // Namespace Events.
}    // Namespace K.
