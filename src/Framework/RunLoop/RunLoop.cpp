////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Framework/RunLoop.h>

#include "SharedState.h"
#include "TimerPort.h"

using std::make_shared;
using std::shared_ptr;

namespace K {
namespace Framework {

RunLoop::RunLoop() {
    sharedState_ = make_shared<SharedState>();
}

shared_ptr<RunLoop::TimerPortInterface> RunLoop::NewTimerPort() {
    return make_shared<TimerPort>(sharedState_);
}

}    // Namespace Framework.
}    // Namespace K.
