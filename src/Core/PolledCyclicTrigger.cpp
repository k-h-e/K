/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/PolledCyclicTrigger.h>

#include <K/Core/Log.h>

using std::chrono::duration_cast;;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using K::Core::Log;

namespace K {
namespace Core {

PolledCyclicTrigger::PolledCyclicTrigger(milliseconds interval)
        : interval_(interval),
          referenceTimePoint_(steady_clock::now()),
          offset_(0),
          elapsed_(0) {
    if (interval_ <= milliseconds(0)) {
        interval_ = milliseconds(1000);
    }
}

bool PolledCyclicTrigger::Check() {
    return Check(steady_clock::now());
}

bool PolledCyclicTrigger::Check(const steady_clock::time_point &now) {
    if (now < referenceTimePoint_) {
        referenceTimePoint_ = now;
        offset_             = elapsed_;
        Log::Print(Log::Level::Warning, this, []{ return "wrap-around detected!"; });
        return false;
    }

    steady_clock::time_point::duration delta = now - referenceTimePoint_;
    if (delta < steady_clock::time_point::duration(0)) {
        referenceTimePoint_ = now;
        offset_             = elapsed_;
        delta               = steady_clock::time_point::duration(0);
        Log::Print(Log::Level::Warning, this, []{ return "wrap-around detected!"; });
        return false;
    }

    steady_clock::time_point::duration newElapsed = delta + offset_;
    if (newElapsed >= elapsed_) {
        elapsed_ = newElapsed;
    } else {
        elapsed_            = steady_clock::time_point::duration::max();
        referenceTimePoint_ = now;
        offset_             = elapsed_;
        Log::Print(Log::Level::Warning, this, []{ return "elapsed time pinning!"; });
    }

    if (elapsed_ >= interval_) {
        while (elapsed_ >= interval_) {
            elapsed_ -= interval_;
        }
        referenceTimePoint_ = now;
        offset_             = elapsed_;
        return true;
    } else {
        return false;
    }
}

milliseconds PolledCyclicTrigger::Remaining() const {
    return duration_cast<milliseconds>(interval_ - elapsed_);
}

void PolledCyclicTrigger::Reset() {
    referenceTimePoint_ = steady_clock::now();
    offset_             = milliseconds(0);
    elapsed_            = milliseconds(0);
}

}    // Namespace Core.
}    // Namespace K.
