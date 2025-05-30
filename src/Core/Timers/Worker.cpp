/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
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

void Timers::Worker::Execute() {
    Log::Print(Log::Level::Debug, this, [&]{ return "spawning..."; });

    shared_->RunTimers();

    Log::Print(Log::Level::Debug, this, [&]{ return "exiting"; });
}

}    // Namespace Core.
}    // Namespace K.
