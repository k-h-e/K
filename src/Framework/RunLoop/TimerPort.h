////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_FRAMEWORK_RUNLOOP_TIMERPORT_H_
#define K_FRAMEWORK_RUNLOOP_TIMERPORT_H_

#include <K/Framework/RunLoop.h>

namespace K {
namespace Framework {

//! Run loop timer port.
/*!
 *  Thread safe (all public instance methods).
 */
class RunLoop::TimerPort : public virtual TimerPortInterface {
  public:
    TimerPort(std::shared_ptr<SharedState> &sharedState);
    TimerPort(const TimerPort &other)            = delete;
    TimerPort &operator=(const TimerPort &other) = delete;
    TimerPort(TimerPort &&other)                 = delete;
    TimerPort &operator=(TimerPort &&other)      = delete;
    ~TimerPort();

    bool OnTimer(int timer) override;

  private:
    std::shared_ptr<SharedState> sharedState_;    // Thread safe.
};

}    // Namespace Framework.
}    // Namespace K.

#endif    // K_FRAMEWORK_RUNLOOP_TIMERPORT_H_
