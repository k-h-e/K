/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_TIMERS_H_
#define K_CORE_TIMERS_H_

#include <chrono>
#include <memory>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

class ThreadPool;

//! Timers.
/*!
 *  The class is thread-safe: all public instance methods are.
 */
class Timers : public virtual Interface {
  public:
    //! Interface to entities handling timers.
    class HandlerInterface : public virtual Interface {
      public:
        //! Asks the recipient object to handle another firing of the specified timer.
        /*!
         *  \return If the handler returns <c>false</c>, the timer will be paused.
         */
        virtual bool OnTimer(int timer) = 0;
    };

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
     *  \param paused
     *  If set, the timer starts in paused state.
     *
     *  \return
     *  Timer ID, required for removal later.
     */
    int AddTimer(std::chrono::milliseconds interval, HandlerInterface *handler, bool paused);
    //! Removes the specified timer.
    /*!
     *  When the method returns, it is guaranteed that the associated handler will not be called again.
     *
     *  Passing an invalid timer ID yields undefined behavior.
     */
    void RemoveTimer(int timer);
    //! Pauses/unpauses the specified timer.
    /*!
     *  In case the timer is paused, it is guaranteed that the associated handler will not be called again when the
     *  method returns.
     *
     *  In case the timer is unpaused, it is reset so its full interval must elapse before it fires again for the first
     *  time.
     */
    void PauseTimer(int timer, bool paused);

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
