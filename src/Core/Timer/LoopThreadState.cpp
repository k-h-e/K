/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "LoopThreadState.h"

using std::shared_ptr;
using K::Core::Timers;

namespace K {
namespace Core {

Timer::LoopThreadState::LoopThreadState(const shared_ptr<RunLoop> &runLoop, const shared_ptr<Timers> &timers)
        : timers(timers),
          runLoop(runLoop),
          runLoopClientId(0),
          timer(0),
          handler(nullptr),
          handlerAssociatedId(0) {
    // Nop.
}

void Timer::LoopThreadState::Activate(bool deepActivation) {
    (void)deepActivation;
    if (handler) {
        handler->OnTimer(handlerAssociatedId);
    }
}

}    // Namespace Core.
}    // Namespace K.
