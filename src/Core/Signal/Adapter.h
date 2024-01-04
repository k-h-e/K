/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SIGNAL_ADAPTER_
#define K_CORE_SIGNAL_ADAPTER_

#include <K/Core/Signal.h>

namespace K {
    namespace Core {
        class RunLoop;
    }
}

namespace K {
namespace Core {

//! Adapter, accessed from an arbitrary thread.
class Signal::Adapter : public virtual Core::Interface {
  public:
    Adapter(const std::shared_ptr<RunLoop> &runLoop, int runLoopClientId);
    Adapter(const Adapter &other)            = delete;
    Adapter &operator=(const Adapter &other) = delete;
    Adapter(Adapter &&other)                 = delete;
    Adapter &operator=(Adapter &&other)      = delete;
    ~Adapter()                               = default;

    void Raise();

  private:
    const std::shared_ptr<RunLoop> runLoop_;
    const int                      runLoopClientId_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SIGNAL_ADAPTER_
