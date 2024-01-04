/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "Adapter.h"

#include <K/Core/RunLoop.h>

using std::shared_ptr;

namespace K {
namespace Core {

Signal::Adapter::Adapter(const shared_ptr<RunLoop> &runLoop, int runLoopClientId)
        : runLoop_(runLoop),
          runLoopClientId_(runLoopClientId) {
    // Nop.
}

void Signal::Adapter::Raise() {
    runLoop_->RequestActivation(runLoopClientId_, false);
}

}    // Namespace Core.
}    // Namespace K.
