#include <kxm/Zarch/Physics/Saucer.h>

#include <kxm/Game/EventLoop.h>
#include <kxm/Zarch/MapParameters.h>
#include <kxm/Zarch/Terrain.h>
#include <kxm/Zarch/Events/ActorCreationEvent.h>
#include <kxm/Zarch/Events/ControlsEvent.h>
#include <kxm/Zarch/Events/MoveEvent.h>
#include <kxm/Zarch/Events/VelocityEvent.h>
#include <kxm/Zarch/Physics/Data.h>

using namespace kxm::Vectoid;

namespace kxm {
namespace Zarch {
namespace Physics {

Saucer::Saucer()
        : body_(this) {
    // Nop.
}

void Saucer::GetTransform(Transform *outTransform) {
    body_.GetTransform(outTransform);
}

void Saucer::GetVelocity(Vector *outVelocity) {
    body_.GetVelocity(outVelocity);
}

void Saucer::Handle(const ActorCreationEvent &event) {
    Actor::Reset(event);
    body_.SetTransform(event.initialTransform);
    body_.SetVelocity(event.initialVelocity);
    body_.DisableGravity();
    body_.DisableAcceleration();
}

void Saucer::Handle(const ControlsEvent &event) {
    static const float maxTravelVelocity = .4f;
    
    Vector velocity;
    body_.GetVelocity(&velocity);
    
    Control control;
    for (int i = 0; i < event.Count(); ++i) {
        event.GetControl(i, &control);
        switch (control.Type()) {
            case Axis1Control:
                velocity.x = control.Argument() * maxTravelVelocity;
                break;
            case Axis2Control:
                velocity.z = control.Argument() * maxTravelVelocity;
                break;
            case ThrusterControl:
                break;
            default:
                break;
        }
    }
    
    body_.SetVelocity(velocity);
}

void Saucer::ExecuteAction() {
    body_.UpdateForTimeStep(data_->updateDeltaTimeS);
}

void Saucer::HandleBodyTransformUpdate(Vectoid::Transform *transform, bool *outVelocityUpdateRequired) {
    Data &data = *data_;
    
    Vector position;
    transform->GetTranslationPart(&position);
    data.mapParameters->xRange.ClampModulo(&position.x);
    data.mapParameters->zRange.ClampModulo(&position.z);
    float terrainHeight = data.terrain->Height(position.x, position.z);
    if (position.y < terrainHeight) {
        position.y = terrainHeight + 5.0f;
    }
    transform->SetTranslationPart(position);

    data.eventLoop->Post(MoveEvent(name_, *transform));
    *outVelocityUpdateRequired = false;
}

void Saucer::HandleBodyVelocityUpdate(Vectoid::Vector *velocity) {
    data_->eventLoop->Post(VelocityEvent(name_, *velocity));
}

}    // Namespace Physics.
}    // Namespace Zarch.
}    // Namespace kxm.
