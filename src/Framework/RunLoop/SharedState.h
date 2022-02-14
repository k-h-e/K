////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_FRAMEWORK_RUNLOOP_SHAREDSTATE_H_
#define K_FRAMEWORK_RUNLOOP_SHAREDSTATE_H_

#include <K/Framework/RunLoop.h>

namespace K {
namespace Framework {

//! Run loop state shared between threads.
/*!
 *  Thread safe (all public instance methods).
 */
class RunLoop::SharedState : public virtual Interface {
  public:
    SharedState()                                    = default;
    SharedState(const SharedState &other)            = delete;
    SharedState &operator=(const SharedState &other) = delete;
    SharedState(SharedState &&other)                 = delete;
    SharedState &operator=(SharedState &&other)      = delete;
    ~SharedState()                                   = default;

  private:
};

}    // Namespace Framework.
}    // Namespace K.

#endif    // K_FRAMEWORK_RUNLOOP_SHAREDSTATE_H_
