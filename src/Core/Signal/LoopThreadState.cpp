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
          handler(nullptr) {
    // Nop.
}

void Signal::LoopThreadState::Activate(bool deepActivation) {
    (void)deepActivation;
    if (handler) {
        handler->OnSignal();
    }
}

}    // Namespace Core.
}    // Namespace K.
