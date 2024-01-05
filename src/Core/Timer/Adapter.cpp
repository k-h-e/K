/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "Adapter.h"

#include <K/Core/RunLoop.h>

using std::shared_ptr;

namespace K {
namespace Core {

Timer::Adapter::Adapter(const shared_ptr<RunLoop> &runLoop, int runLoopClientId)
        : runLoop_(runLoop),
          runLoopClientId_(runLoopClientId) {
    // Nop.
}

bool Timer::Adapter::OnTimer(int timer) {
    (void)timer;
    runLoop_->RequestActivation(runLoopClientId_, false);
    return true;
}

}    // Namespace Core.
}    // Namespace K.
