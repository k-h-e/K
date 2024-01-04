/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_TIMER_ADAPTER_
#define K_CORE_TIMER_ADAPTER_

#include <K/Core/Timers.h>
#include <K/Core/Timer.h>

namespace K {
    namespace Core {
        class RunLoop;
    }
}

namespace K {
namespace Core {

//! Adapter, accessed from a <c>Core::Timers</c> worker thread.
class Timer::Adapter : public virtual Core::Timers::HandlerInterface {
  public:
    Adapter(const std::shared_ptr<RunLoop> &runLoop, int runLoopClientId);
    Adapter(const Adapter &other)            = delete;
    Adapter &operator=(const Adapter &other) = delete;
    Adapter(Adapter &&other)                 = delete;
    Adapter &operator=(Adapter &&other)      = delete;
    ~Adapter()                               = default;

    bool OnTimer(int timer) override;

  private:
    const std::shared_ptr<RunLoop> runLoop_;
    int                            runLoopClientId_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TIMER_ADAPTER_
