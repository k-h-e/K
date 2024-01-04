/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_STOPWATCH_H_
#define K_CORE_STOPWATCH_H_

#include <chrono>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Simple stop watch to measure elapsed time.
class StopWatch : public virtual Interface {
  public:
    StopWatch();
    StopWatch(const StopWatch &other)            = delete;
    StopWatch &operator=(const StopWatch &other) = delete;
    StopWatch(StopWatch &&other)                 = delete;
    StopWatch &operator=(StopWatch &&other)      = delete;
    ~StopWatch()                                 = default;

    //! Restarts the stop watch.
    void Restart();
    //! Tells the elapsed time in milliseconds.
    int ElapsedMs();
    //! Tells the elapsed time in milliseconds since the last call to <c>DeltaMs()</c>.
    int DeltaMs();

  private:
    std::chrono::steady_clock::time_point startTime_;
    int                                   deltaLastElapsedMs_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_STOPWATCH_H_

