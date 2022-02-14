////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Framework/Timer.h>

using std::shared_ptr;

namespace K {
namespace Framework {

Timer::Timer(const shared_ptr<RunLoop> &runLoop) {
    timerPort_ = runLoop->NewTimerPort();
}

Timer::~Timer() {
    // Nop, for now.
}

}    // Namespace Framework.
}    // Namespace K.
