/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "LoopThreadState.h"

using std::shared_ptr;

namespace K {
namespace Core {

Signal::LoopThreadState::LoopThreadState(const shared_ptr<RunLoop> &runLoop)
        : runLoop(runLoop),
          runLoopClientId(0),
          handler(nullptr),
          handlerAssociatedId(0) {
    // Nop.
}

void Signal::LoopThreadState::Activate(bool deepActivation) {
    (void)deepActivation;
    if (handler) {
        handler->OnSignal(handlerAssociatedId);
    }
}

}    // Namespace Core.
}    // Namespace K.
