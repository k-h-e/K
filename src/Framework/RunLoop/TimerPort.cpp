////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TimerPort.h"

using std::shared_ptr;

namespace K {
namespace Framework {

RunLoop::TimerPort::TimerPort(shared_ptr<SharedState> &sharedState)
        : sharedState_(sharedState) {
    // Nop.
}

RunLoop::TimerPort::~TimerPort() {

}

bool RunLoop::TimerPort::OnTimer(int timer) {
    (void)timer;
    return true;
}

}    // Namespace Framework.
}    // Namespace K.
