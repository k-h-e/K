////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SharedState.h"

#include <K/Core/Log.h>

using std::mutex;
using std::to_string;
using std::unique_lock;
using std::vector;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using K::Core::Log;

namespace K {
namespace Core {

Timers::SharedState::SharedState()
        : timers_(1),
          shutDownRequested_(false),
          workerFinished_(false) {
    // Nop.
}

void Timers::SharedState::RequestShutDown() {
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    shutDownRequested_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

void Timers::SharedState::WaitForWorkerFinished() {
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    while (!workerFinished_) {
        stateChanged_.wait(critical);
    }
}    // ......................................................................................... critical section, end.

int Timers::SharedState::AddTimer(milliseconds interval, TimerHandlerInterface *handler) {
    if (interval <= milliseconds(0)) {
        interval = milliseconds(1000);
    }
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    int       id;
    TimerInfo &info = timers_.Get(0, &id);
    info = TimerInfo(interval, handler, id);
    stateChanged_.notify_all();
    Log::Print(Log::Level::Debug, this, [&]{
        return "timer " + to_string(id) + " added, num_now=" + to_string(timers_.Count() - timers_.IdleCount());
    });
    return id;
}    // ......................................................................................... critical section, end.

void Timers::SharedState::RemoveTimer(int timer) {
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    timers_.Put(timer);
    stateChanged_.notify_all();
    Log::Print(Log::Level::Debug, this, [&]{
        return "timer " + to_string(timer) + " removed, num_remaining="
            + to_string(timers_.Count() - timers_.IdleCount());
    });
}    // ......................................................................................... critical section, end.

void Timers::SharedState::RunTimers() {
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    while (!shutDownRequested_) {
        if (timers_.Count() - timers_.IdleCount() > 0) {
            steady_clock::time_point now           = steady_clock::now();
            milliseconds             timeUntilNext = milliseconds::max();
            for (TimerInfo &info : timers_.Iterate(0)) {
                if (info.trigger.Check(now)) {
                    info.handler->OnTimer(info.timerId);
                }
                milliseconds remaining = info.trigger.Remaining();
                if (remaining < timeUntilNext) {
                    timeUntilNext = remaining;
                }
            }

            steady_clock::time_point afterTimers = steady_clock::now();
            milliseconds handlerTime(0);
            if (afterTimers > now) {
                milliseconds deltaTime = duration_cast<milliseconds>(afterTimers - now);
                if (deltaTime > milliseconds(0)) {
                    handlerTime = deltaTime;
                }
            }

            if (timeUntilNext > handlerTime) {
                timeUntilNext = timeUntilNext - handlerTime;
                milliseconds toWait = timeUntilNext + milliseconds(2);
                if (!(toWait > timeUntilNext)) {
                    toWait = timeUntilNext;
                    Log::Print(Log::Level::Warning, this, [&]{
                        return "to_wait_corrected=" + to_string(toWait.count()) + "ms";
                    });
                }
                stateChanged_.wait_for(critical, toWait);
            }
        } else {
            Log::Print(Log::Level::Debug, this, [&]{ return "sleeping until timers present..."; });
            stateChanged_.wait(critical);
        }
    }
}    // ......................................................................................... critical section, end.

void Timers::SharedState::OnCompletion(int completionId) {
    (void)completionId;
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    workerFinished_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

Timers::SharedState::TimerInfo::TimerInfo()
        : trigger(milliseconds(1000)),
          handler(nullptr),
          timerId(0) {
    // Nop.
}

Timers::SharedState::TimerInfo::TimerInfo(milliseconds anInterval, TimerHandlerInterface *aHandler, int aTimerId)
        : trigger(anInterval),
          handler(aHandler),
          timerId(aTimerId) {
    // Nop.
}

}    // Namespace Core.
}    // Namespace K.
