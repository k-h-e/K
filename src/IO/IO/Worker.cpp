#include "Worker.h"

#include <K/Core/Log.h>
#include "SharedState.h"

using std::shared_ptr;
using K::Core::Log;

namespace K {
namespace IO {

IO::Worker::Worker(shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState) {
    // Nop.
}

void IO::Worker::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

}    // Namespace IO.
}    // Namespace K.

