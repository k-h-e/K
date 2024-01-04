/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

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
        : timers_(2),
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

int Timers::SharedState::AddTimer(milliseconds interval, HandlerInterface *handler, bool paused) {
    if (interval <= milliseconds(0)) {
        interval = milliseconds(1000);
    }
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    int       id;
    TimerInfo &info = timers_.Get(activeTimers, &id);
    info = TimerInfo(interval, handler, id);

    if (paused) {
        timers_.Move(id, pausedTimers);
        Log::Print(Log::Level::Debug, this, [&]{
            return "timer " + to_string(id) + " added as paused, num_now="
                       + to_string(timers_.Count() - timers_.IdleCount());
        });
    } else {
        Log::Print(Log::Level::Debug, this, [&]{
            return "timer " + to_string(id) + " added, num_now=" + to_string(timers_.Count() - timers_.IdleCount());
        });
    }

    stateChanged_.notify_all();
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

void Timers::SharedState::PauseTimer(int timer, bool paused) {
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    if (paused) {
        timers_.Move(timer, pausedTimers);
        Log::Print(Log::Level::Debug, this, [&]{ return "timer " + to_string(timer) + " paused"; });
    } else {
        timers_.Item(timer).trigger.Reset();
        timers_.Move(timer, activeTimers);
        Log::Print(Log::Level::Debug, this, [&]{ return "timer " + to_string(timer) + " unpaused"; });
    }
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

void Timers::SharedState::RunTimers() {
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    vector<int> timersToPause;
    while (!shutDownRequested_) {
        if (!timers_.Empty(activeTimers)) {
            steady_clock::time_point now           = steady_clock::now();
            milliseconds             timeUntilNext = milliseconds::max();

            timersToPause.clear();
            for (TimerInfo &info : timers_.Iterate(activeTimers)) {
                if (info.trigger.Check(now)) {
                    bool handlerDidRequestPause = !info.handler->OnTimer(info.timerId);
                    if (handlerDidRequestPause) {
                        timersToPause.push_back(info.timerId);
                    }
                }
                milliseconds remaining = info.trigger.Remaining();
                if (remaining < timeUntilNext) {
                    timeUntilNext = remaining;
                }
            }

            for (int timer : timersToPause) {
                timers_.Move(timer, pausedTimers);
                Log::Print(Log::Level::Debug, this, [&]{
                    return "timer " + to_string(timer) + " paused upon handler request";
                });
            }

            milliseconds toWait(0);
            if (!timers_.Empty(activeTimers)) {
                steady_clock::time_point afterTimers = steady_clock::now();
                milliseconds handlerTime(0);
                if (afterTimers > now) {
                    milliseconds deltaTime = duration_cast<milliseconds>(afterTimers - now);
                    if (deltaTime > milliseconds(0)) {
                        handlerTime = deltaTime;
                    }
                }

                if (timeUntilNext > handlerTime) {
                    toWait = timeUntilNext - handlerTime;
                }
            }

            milliseconds newToWait = toWait + milliseconds(2);
            if (newToWait > toWait) {
                toWait = newToWait;
            }

            stateChanged_.wait_for(critical, toWait);

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

Timers::SharedState::TimerInfo::TimerInfo(milliseconds anInterval, HandlerInterface *aHandler, int aTimerId)
        : trigger(anInterval),
          handler(aHandler),
          timerId(aTimerId) {
    // Nop.
}

}    // Namespace Core.
}    // Namespace K.
