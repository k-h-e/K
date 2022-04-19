#include "LoopThreadState.h"

using std::shared_ptr;
using K::Core::Framework::RunLoop;

namespace K {
namespace Events {
namespace Framework {

EventNotifier::LoopThreadState::LoopThreadState(const shared_ptr<EventHub> &hub, int clientLoopId,
                                                const shared_ptr<RunLoop> &runLoop)
        : hub(hub),
          clientLoopId(clientLoopId),
          runLoop(runLoop),
          runLoopClientId(0),
          handler(nullptr),
          handlerAssociatedId(0) {
    // Nop.
}

void EventNotifier::LoopThreadState::Activate(bool deepActivation) {
    (void)deepActivation;
    if (handler) {
        handler->OnEventsAvailable(handlerAssociatedId);
    }
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.
