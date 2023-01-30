#include <K/Core/StopWatch.h>

#include <K/Core/Log.h>

using std::chrono::milliseconds;
using std::chrono::steady_clock;
using K::Core::Log;

namespace K {
namespace Core {

StopWatch::StopWatch()
        : startTime_{steady_clock::now()} {
    // Nop.
};

void StopWatch::Restart() {
    startTime_ = steady_clock::now();
}

int StopWatch::ElapsedMs() {
    steady_clock::time_point now = steady_clock::now();
    if (now >= startTime_) {
        auto counts = duration_cast<milliseconds>(now - startTime_).count();
        if (counts >= 0) {
            int timeMs = static_cast<int>(counts);
            if (timeMs >= 0) {
                return timeMs;
            }
        }
    }

    Log::Print(Log::Level::Warning, this, [&]{ return "overflow detected, stop watch restarted!"; });
    Restart();
    return 0;
}

}    // Namespace Core.
}    // Namespace K.
