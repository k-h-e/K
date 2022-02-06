////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_POLLEDCYCLICTRIGGER_H_
#define K_CORE_POLLEDCYCLICTRIGGER_H_

#include <chrono>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Can be asked whether to trigger a cyclic action.
class PolledCyclicTrigger : public virtual Interface {
  public:
    PolledCyclicTrigger(std::chrono::milliseconds interval);
    PolledCyclicTrigger()                                            = delete;
    PolledCyclicTrigger(const PolledCyclicTrigger &other)            = default;
    PolledCyclicTrigger &operator=(const PolledCyclicTrigger &other) = default;
    PolledCyclicTrigger(PolledCyclicTrigger &&other)                 = default;
    PolledCyclicTrigger &operator=(PolledCyclicTrigger &&other)      = default;

    //! Checks and tells whether the next triggering of the cyclic action is due.
    bool Check();
    //! Like <c>Check()</c>, but the now-time is passed in.
    bool Check(const std::chrono::steady_clock::time_point &now);
    //! Returns the time remaining until the next triggering of the cyclic action is due, measured from the time of the
    //! last check.
    std::chrono::milliseconds Remaining() const;

  private:
    std::chrono::steady_clock::time_point::duration interval_;
    std::chrono::steady_clock::time_point           referenceTimePoint_;
    std::chrono::steady_clock::time_point::duration offset_;
    std::chrono::steady_clock::time_point::duration elapsed_;               // Always: 0 <= elapsed_ < interval_ > 0.
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_POLLEDCYCLICTRIGGER_H_
