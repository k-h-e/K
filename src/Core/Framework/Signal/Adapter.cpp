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

Signal::Adapter::Adapter(const shared_ptr<RunLoop> &runLoop, int runLoopClientId)
        : runLoop_(runLoop),
          runLoopClientId_(runLoopClientId) {
    // Nop.
}

void Signal::Adapter::Raise() {
    runLoop_->RequestActivation(runLoopClientId_, false);
}

}    // Namespace Framework.
}    // Namespace Core.
}    // Namespace K.
