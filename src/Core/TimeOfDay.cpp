////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/TimeOfDay.h>

#include <sys/time.h>
#include <K/Core/BlockingInStreamInterface.h>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/NumberTools.h>

using std::string;

namespace K {
namespace Core {

TimeOfDay::TimeOfDay()
        : hours_(0),
          minutes_(0),
          seconds_(0),
          milliSeconds_(0) {
    // Nop.
}

TimeOfDay::TimeOfDay(int hours, int minutes, int seconds, int milliSeconds)
        : hours_(hours),
          minutes_(minutes),
          seconds_(seconds),
          milliSeconds_(milliSeconds) {
    NumberTools::Clamp(&hours_,        0,  23);
    NumberTools::Clamp(&minutes_,      0,  59);
    NumberTools::Clamp(&seconds_,      0,  59);
    NumberTools::Clamp(&milliSeconds_, 0, 999);
}

string TimeOfDay::ToString() const {
    char line[80];
    std::snprintf(line, 80, "%02d:%02d:%02d.%03d", hours_, minutes_, seconds_, milliSeconds_);
    return string(line);
}

void TimeOfDay::Serialize(BlockingOutStreamInterface *stream) const {
    (*stream) << hours_;
    (*stream) << minutes_;
    (*stream) << seconds_;
    (*stream) << milliSeconds_;
}

void TimeOfDay::Deserialize(BlockingInStreamInterface *stream) {
    (*stream) >> hours_;
    (*stream) >> minutes_;
    (*stream) >> seconds_;
    (*stream) >> milliSeconds_;
}

TimeOfDay TimeOfDay::NowUtc() {
    struct timeval now;
    gettimeofday(&now, nullptr);

    time_t nowAsTimeT = static_cast<time_t>(now.tv_sec);
    tm utc = *gmtime(&nowAsTimeT);

    return TimeOfDay(utc.tm_hour, utc.tm_min, utc.tm_sec, (static_cast<int>(now.tv_usec) + 500) / 1000);
}

}    // Namespace Core.
}    // Namespace K.
