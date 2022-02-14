////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_FRAMEWORK_RUNLOOP_H_
#define K_FRAMEWORK_RUNLOOP_H_

#include <memory>

#include <K/Core/Interface.h>

namespace K {
namespace Framework {

//! Run loop.
class RunLoop : public virtual Core::Interface {
  public:
    //! Interface to a run loop timer port.
    class TimerPortInterface : public virtual Interface {
      public:
    };

    RunLoop();
    RunLoop(const RunLoop &other)            = delete;
    RunLoop &operator=(const RunLoop &other) = delete;
    RunLoop(RunLoop &&other)                 = delete;
    RunLoop &operator=(RunLoop &&other)      = delete;
    ~RunLoop()                               = default;

    //! Creates a run loop port for a new timer and returns it.
    /*!
     *  The returned timer port is thread safe.
     */
    std::shared_ptr<TimerPortInterface> NewTimerPort();

  private:
    class SharedState;
    class TimerPort;

    std::shared_ptr<SharedState> sharedState_;    // Thread safe.
};

}    // Namespace Framework.
}    // Namespace K.

#endif    // K_FRAMEWORK_RUNLOOP_H_
