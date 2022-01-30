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

#include <K/Core/ActionInterface.h>
#include <K/Core/Log.h>
#include <K/Core/StopWatch.h>

using std::mutex;
using std::to_string;
using std::unique_lock;
using std::vector;
using std::chrono::milliseconds;
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

int Timers::SharedState::AddTimer(milliseconds interval, ActionInterface *handler) {
    if (interval <= milliseconds(0)) {
        interval = milliseconds(1000);
    }
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    int id;
    timers_.Get(0, &id) = TimerInfo(interval, handler);
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
    vector<ActionInterface *> handlersToCall;
    StopWatch stopWatch;
    stopWatch.Start();
    while (!shutDownRequested_) {
        if (timers_.Count() - timers_.IdleCount() > 0) {
            handlersToCall.clear();
            milliseconds elapsed       = stopWatch.Elapsed();
            milliseconds timeUntilNext = milliseconds::max();
            for (TimerInfo &info : timers_.Iterate(0)) {
                if (elapsed >= info.remaining) {
                    handlersToCall.push_back(info.handler);
                    milliseconds elapsedOfNext = elapsed - info.remaining;
                    while (elapsedOfNext >= info.interval) {
                        elapsedOfNext -= info.interval;
                    }
                    info.remaining = info.interval - elapsedOfNext;
                } else {
                    info.remaining -= elapsed;
                }

                if (info.remaining < timeUntilNext) {
                    timeUntilNext = info.remaining;
                }
            }

            if (!handlersToCall.empty()) {
                for (ActionInterface *handler : handlersToCall) {
                    handler->ExecuteAction();
                }
            }

            milliseconds deltaTime = stopWatch.Elapsed() - elapsed;
            timeUntilNext = (timeUntilNext > deltaTime) ? timeUntilNext - deltaTime : milliseconds(0);
            if (timeUntilNext > milliseconds(0)) {
                milliseconds toWait = timeUntilNext + milliseconds(1);
                if (toWait < timeUntilNext) {
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

        stopWatch.ClearElapsed();
    }
}    // ......................................................................................... critical section, end.

void Timers::SharedState::OnCompletion(int completionId) {
    (void)completionId;
    unique_lock<mutex> critical(lock_);    // critical section..........................................................
    workerFinished_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

Timers::SharedState::TimerInfo::TimerInfo()
        : interval(1000),
          remaining(1000),
          handler(nullptr) {
    // Nop.
}

Timers::SharedState::TimerInfo::TimerInfo(milliseconds anInterval, ActionInterface *aHandler)
        : handler(aHandler) {
    if (anInterval < milliseconds(1)) {
        anInterval = milliseconds(1000);
    }

    interval  = anInterval;
    remaining = interval;
}

}    // Namespace Core.
}    // Namespace K.
