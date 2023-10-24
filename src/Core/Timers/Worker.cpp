/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "Worker.h"

#include <K/Core/Log.h>
#include "SharedState.h"

using std::shared_ptr;
using std::vector;
using K::Core::Log;

namespace K {
namespace Core {

Timers::Worker::Worker(const shared_ptr<SharedState> &shared)
        : shared_(shared) {
    // Nop.
}

void Timers::Worker::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, [&]{ return "spawning..."; });

    shared_->RunTimers();

    Log::Print(Log::Level::Debug, this, [&]{ return "exiting"; });
}

}    // Namespace Core.
}    // Namespace K.
