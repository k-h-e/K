/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_TIMER_H_
#define K_CORE_TIMER_H_

#include <chrono>
#include <memory>
#include <K/Core/Interface.h>

namespace K {
    namespace Core {
        class Timers;
        class RunLoop;
    }
}

namespace K {
namespace Core {

//! Timer, for use with a <c>RunLoop</c>.
class Timer : public virtual Core::Interface {
  public:
    class HandlerInterface : public virtual Core::Interface {
      public:
        //! Called when(ever) the timer fires.
        virtual void OnTimer() = 0;
    };

    Timer(std::chrono::milliseconds interval, const std::shared_ptr<RunLoop> &runLoop,
          const std::shared_ptr<Core::Timers> &timers);
    Timer()                              = delete;
    Timer(const Timer &other)            = delete;
    Timer &operator=(const Timer &other) = delete;
    Timer(Timer &&other)                 = delete;
    Timer &operator=(Timer &&other)      = delete;
    ~Timer();

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the timer.
     *
     *  The handler is expected to outlive the timer.
     */
    void Register(HandlerInterface *handler);

  private:
    class  Adapter;
    struct LoopThreadState;

    const std::unique_ptr<LoopThreadState> loopThreadState_;
    std::unique_ptr<Adapter>               adapter_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TIMER_H_
