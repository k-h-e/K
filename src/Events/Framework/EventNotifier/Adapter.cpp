/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "Adapter.h"

#include <K/Core/Framework/RunLoop.h>

namespace K {
namespace Events {
namespace Framework {

EventNotifier::Adapter::Adapter(const std::shared_ptr<Core::Framework::RunLoop> &runLoop, int runLoopClientId)
        : runLoop_(runLoop),
          runLoopClientId_(runLoopClientId) {
    // Nop.
}

void EventNotifier::Adapter::OnEventsAvailable() {
    runLoop_->RequestActivation(runLoopClientId_, false);
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.
