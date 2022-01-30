////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_TIMERS_H_
#define K_CORE_TIMERS_H_

#include <chrono>
#include <memory>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

class ActionInterface;
class ThreadPool;

//! Timers.
/*!
 *  The class is thread-safe: all public instance methods are.
 */
class Timers : public virtual Interface {
  public:
    Timers(const std::shared_ptr<ThreadPool> &threadPool);
    Timers()                               = delete;
    Timers(const Timers &other)            = delete;
    Timers &operator=(const Timers &other) = delete;
    Timers(Timers &&other)                 = delete;
    Timers &operator=(Timers &&other)      = delete;
    ~Timers();

    //! Adds a new timer.
    /*!
     *  \param handler
     *  The handler will get called on a worker thread. It must not call back into the <c>Timers</c> object.
     *
     *  \return
     *  Timer ID, required for removal later.
     */
    int AddTimer(std::chrono::milliseconds interval, ActionInterface *handler);
    //! Removes the specified timer.
    /*!
     *  When the method returns, it is guaranteed that the associated handler will not be called again.
     *
     *  Passing an invalid timer ID yields undefined behavior.
     */
    void RemoveTimer(int timer);

  private:
    class SharedState;
    class Worker;

    std::shared_ptr<SharedState> shared_;
    std::shared_ptr<ThreadPool>  threadPool_;

    std::shared_ptr<Worker>      worker_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TIMERS_H_
