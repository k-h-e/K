/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_TIMERS_SHAREDSTATE_H_
#define K_CORE_TIMERS_SHAREDSTATE_H_

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/Core/PolledCyclicTrigger.h>
#include <K/Core/ReusableItems.h>
#include <K/Core/Timers.h>

namespace K {
namespace Core {

//! Shared state for the timers implementation.
/*!
 *  Thread-safe (all public methods).
 */
class Timers::SharedState : public virtual CompletionHandlerInterface {
  public:
    SharedState();
    SharedState(const SharedState &other)            = delete;
    SharedState &operator=(const SharedState &other) = delete;
    SharedState(SharedState &&other)                 = delete;
    SharedState &operator=(SharedState &&other)      = delete;

    void RequestShutDown();
    void WaitForWorkerFinished();
    int AddTimer(std::chrono::milliseconds interval, HandlerInterface *handler, bool paused);
    void RemoveTimer(int timer);
    void PauseTimer(int timer, bool paused);
    void RunTimers();
    void OnCompletion(int completionId) override;

  private:
    static const int activeTimers = 0;
    static const int pausedTimers = 1;
    struct TimerInfo {
        K::Core::PolledCyclicTrigger trigger;
        HandlerInterface             *handler;
        int                          timerId;

        TimerInfo();
        TimerInfo(std::chrono::milliseconds anInterval, HandlerInterface *aHandler, int aTimerId);
        TimerInfo(const TimerInfo &other)            = default;
        TimerInfo &operator=(const TimerInfo &other) = default;
        TimerInfo(TimerInfo &&other)                 = default;
        TimerInfo &operator=(TimerInfo &&other)      = default;
    };

    std::mutex                        lock_;
    std::condition_variable           stateChanged_;
    K::Core::ReusableItems<TimerInfo> timers_;
    bool                              shutDownRequested_;
    bool                              workerFinished_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_THREADPOOL_SHAREDSTATE_H_
