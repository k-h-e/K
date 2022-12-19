////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoopThreadState.h"

using std::shared_ptr;

namespace K {
namespace Core {
namespace Framework {

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

}    // Namespace Framework.
}    // Namespace Core.
}    // Namespace K.
