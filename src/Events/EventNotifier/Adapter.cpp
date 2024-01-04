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
using K::Core::RunLoop;

namespace K {
namespace Events {

EventNotifier::Adapter::Adapter(const shared_ptr<RunLoop> &runLoop, int runLoopClientId)
        : runLoop_(runLoop),
          runLoopClientId_(runLoopClientId) {
    // Nop.
}

void EventNotifier::Adapter::OnEventsAvailable() {
    runLoop_->RequestActivation(runLoopClientId_, false);
}

}    // Namespace Events.
}    // Namespace K.
