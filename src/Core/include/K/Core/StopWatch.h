////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_STOPWATCH_H_
#define K_CORE_STOPWATCH_H_

#include <chrono>

namespace K {
namespace Core {

//! Stop watch.
class StopWatch {
  public:
    StopWatch();
    StopWatch(const StopWatch &other)            = delete;
    StopWatch &operator=(const StopWatch &other) = delete;
    StopWatch(StopWatch &&other)                 = delete;
    StopWatch &operator=(StopWatch &&other)      = delete;

    //! (Re)starts the stop watch.
    void Start();
    //! Stops the stop watch.
    void Stop();
    //! Stops the stop watch if it is running and resets the elapsed time to 0ms.
    void Reset();
    //! Tells the elapsed time.
    std::chrono::milliseconds Elapsed();
    //! Tells whether the next instance of a cyclically repeating event is due.
    /*!
     *  Potentially turns back the elapsed time.
     */
    bool CyclicCheck(std::chrono::milliseconds interval, std::chrono::milliseconds *outRemaining);

  private:
    void UpdateElapsed();

    std::chrono::steady_clock::time_point lastTime_;
    bool                                  running_;
    std::chrono::milliseconds             elapsed_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_STOPWATCH_H_
