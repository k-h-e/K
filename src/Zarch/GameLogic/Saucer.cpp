#include <kxm/Zarch/GameLogic/Saucer.h>

#include <kxm/Game/EventLoop.h>
#include <kxm/Zarch/GameLogic/Data.h>
#include <kxm/Zarch/Events/ActorCreationEvent.h>
#include <kxm/Zarch/Events/ControlsRequestEvent.h>
#include <kxm/Zarch/Events/ControlsEvent.h>

using namespace std;
using namespace kxm::Game;

namespace kxm {
namespace Zarch {
namespace GameLogic {

Saucer::Saucer() {
    // Nop.
}

void Saucer::Handle(const ActorCreationEvent &event) {
    Actor::Reset(event);
}

void Saucer::Handle(const ControlsRequestEvent &event) {
    ControlsEvent newEvent(name_);
    Control       control;
    for (int i = 0; i < event.Count(); ++i) {
        event.GetControl(i, &control);
        switch (control.Type()) {
            default:
                newEvent.AddControl(control);
                break;
        }
    }
    if (newEvent.Count()) {
        data_->eventLoop->Post(newEvent);
    }
}

void Saucer::ExecuteAction() {
    // Nop.
}

}    // Namespace GameLogic.
}    // Namespace Zarch.
}    // Namespace kxm.