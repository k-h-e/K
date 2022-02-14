////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_FRAMEWORK_TIMER_H_
#define K_FRAMEWORK_TIMER_H_

#include <K/Framework/RunLoop.h>

namespace K {
namespace Framework {

//! Timer, to be used on a thread managed by a <c>RunLoop</c>.
class Timer : public virtual Core::Interface {
  public:
    Timer(const std::shared_ptr<RunLoop> &runLoop);
    Timer(const Timer &other)            = delete;
    Timer &operator=(const Timer &other) = delete;
    Timer(Timer &&other)                 = delete;
    Timer &operator=(Timer &&other)      = delete;
    ~Timer();

  private:
    std::shared_ptr<RunLoop::TimerPortInterface> timerPort_;    // Thread safe.
};

}    // Namespace Framework.
}    // Namespace K.

#endif    // K_FRAMEWORK_TIMER_H_
