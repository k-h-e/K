////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/StopWatch.h>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::steady_clock;

namespace K {
namespace Core {

StopWatch::StopWatch()
        : lastTime_(steady_clock::now()),
          running_(false),
          elapsed_(0) {
    // Nop.
}

void StopWatch::Start() {
    if (!running_) {
        lastTime_ = steady_clock::now();
        running_  = true;
    } else {
        UpdateElapsed();
    }
}

void StopWatch::Stop() {
    if (running_) {
        UpdateElapsed();
        running_ = false;
    }
}

void StopWatch::Reset() {
    elapsed_ = milliseconds(0);
    running_ = false;
}

void StopWatch::ClearElapsed() {
    elapsed_ = milliseconds(0);
}

std::chrono::milliseconds StopWatch::Elapsed() {
    if (running_) {
        UpdateElapsed();
    }
    return elapsed_;
}

bool StopWatch::CyclicCheck(milliseconds interval, milliseconds *outRemaining) {
    bool fired = false;

    if (interval > milliseconds(0)) {
        if (!running_) {
            Reset();
            Start();
        } else {
            UpdateElapsed();
            if (elapsed_ >= interval) {
                while (elapsed_ >= interval) {
                    elapsed_ -= interval;
                }

                fired = true;
            }
        }

        *outRemaining = interval - elapsed_;

    } else {
        *outRemaining = milliseconds(0);
    }

    return fired;
}

void StopWatch::UpdateElapsed() {
    steady_clock::time_point now = steady_clock::now();
    if (now < lastTime_) {    // Wrap-around paranoia.
        lastTime_ = now;
    }

    milliseconds delta = duration_cast<milliseconds>(now - lastTime_);
    if (delta < milliseconds(0)) {    // Wrap-around paranoia.
        lastTime_ = now;
        delta = milliseconds(0);
    }

    elapsed_ += delta;
    if (elapsed_ < milliseconds(0)) {    // Wrap-around paranoia.
        elapsed_ = milliseconds(0);
    }

    lastTime_ = now;
}

}    // Namespace Core.
}    // Namespace K.
