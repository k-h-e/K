/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SIGNAL_H_
#define K_CORE_SIGNAL_H_

#include <memory>
#include <K/Core/Interface.h>

namespace K {
    namespace Core {
        class RunLoop;
    }
}

namespace K {
namespace Core {

//! Signal, for use with a <c>RunLoop</c>.
/*!
 *  Arbitrary threads can raise the signal, causing the signal's handler to be invoked on the run loop thread.
 */
class Signal : public virtual Core::Interface {
  public:
    class HandlerInterface : public virtual Core::Interface {
      public:
        //! Called when the signal has been raised.
        virtual void OnSignal() = 0;
    };

    Signal(const std::shared_ptr<RunLoop> &runLoop);
    Signal()                               = delete;
    Signal(const Signal &other)            = delete;
    Signal &operator=(const Signal &other) = delete;
    Signal(Signal &&other)                 = delete;
    Signal &operator=(Signal &&other)      = delete;
    ~Signal();

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the signal.
     *
     *  The handler is expected to outlive the signal.
     */
    void Register(HandlerInterface *handler);
    //! <b>[Thread-safe]</b> Raises the signal.
    void Raise();

  private:
    class  Adapter;
    struct LoopThreadState;

    const std::unique_ptr<LoopThreadState> loopThreadState_;
    std::unique_ptr<Adapter>               adapter_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SIGNAL_H_
