////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Adapter.h"

#include <K/Core/Framework/RunLoop.h>

using std::shared_ptr;

namespace K {
namespace Core {
namespace Framework {

Timer::Adapter::Adapter(const shared_ptr<RunLoop> &runLoop, int runLoopClientId)
        : runLoop_(runLoop),
          runLoopClientId_(runLoopClientId) {
    // Nop.
}

bool Timer::Adapter::OnTimer(int timer) {
    (void)timer;
    runLoop_->RequestActivation(runLoopClientId_, false);
    return true;
}

}    // Namespace Framework.
}    // Namespace Core.
}    // Namespace K.
