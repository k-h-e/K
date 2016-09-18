#include <kxm/Zarch/Video/Video.h>

#include <kxm/Core/logging.h>
#include <kxm/Vectoid/PerspectiveProjection.h>
#include <kxm/Vectoid/Camera.h>
#include <kxm/Vectoid/CoordSys.h>
#include <kxm/Vectoid/Geode.h>
#include <kxm/Vectoid/Particles.h>
#include <kxm/Vectoid/ParticlesRenderer.h>
#include <kxm/Vectoid/Transform.h>
#include <kxm/Game/EventLoop.h>
#include <kxm/Game/Actions.h>
#include <kxm/Zarch/LanderGeometry.h>
#include <kxm/Zarch/MapParameters.h>
#include <kxm/Zarch/Terrain.h>
#include <kxm/Zarch/Video/Shot.h>
#include <kxm/Zarch/Video/StarField.h>
#include <kxm/Zarch/Video/TerrainRenderer.h>
#include <kxm/Zarch/Events/ZarchEvent.h>
#include <kxm/Zarch/Events/ActorCreationEvent.h>
#include <kxm/Zarch/Events/ActorTerminationEvent.h>
#include <kxm/Zarch/Events/FrameGeneratedEvent.h>
#include <kxm/Zarch/Events/MoveEvent.h>
#include <kxm/Zarch/Events/VelocityEvent.h>
#include <kxm/Zarch/Events/AccelerationEvent.h>
#include <kxm/Zarch/Events/ControlsEvent.h>

using namespace std;
using namespace std::chrono;
using namespace kxm::Vectoid;
using namespace kxm::Game;

namespace kxm {
namespace Zarch {
namespace Video {

Video::Video(shared_ptr<EventLoop<ZarchEvent, EventHandlerCore>> eventLoop)
        : eventLoop_(eventLoop),
          actions_(new Actions()),
          landers_(actions_),
          shots_(actions_),
          lastFrameTime_(steady_clock::now()) {
    eventLoop_->RegisterHandler(ActorCreationEvent::type,    this);
    eventLoop_->RegisterHandler(ActorTerminationEvent::type, this);
    eventLoop_->RegisterHandler(FrameGeneratedEvent::type,   this);
    eventLoop_->RegisterHandler(MoveEvent::type,             this);
    eventLoop_->RegisterHandler(VelocityEvent::type,         this);
    eventLoop_->RegisterHandler(AccelerationEvent::type,     this);
    
    data_ = make_shared<Data>();
    data_->mapParameters = make_shared<MapParameters>();
    data_->terrain       = make_shared<Terrain>(data_->mapParameters);
    
    // Install scene graph...
    data_->projection = make_shared<PerspectiveProjection>();
    data_->projection->SetWindowSize(11.0f);
    data_->projection->SetViewingDepth(11.0f);
    data_->projection->SetEyepointDistance(11.0f);
    data_->camera = make_shared<Camera>();
    data_->projection->AddChild(data_->camera);
    
    data_->terrainRenderer = make_shared<TerrainRenderer>(data_->terrain, data_->mapParameters);
    data_->camera->AddChild(make_shared<Geode>(data_->terrainRenderer));
    
    shared_ptr<Particles> shotParticles(new Particles()),
                          starFieldParticles(new Particles());
    data_->shotParticles = shotParticles;
    data_->camera->AddChild(make_shared<Geode>(make_shared<ParticlesRenderer>(shotParticles)));
    data_->camera->AddChild(make_shared<Geode>(make_shared<ParticlesRenderer>(starFieldParticles)));
    
    starField_ = unique_ptr<StarField>(new StarField(data_, starFieldParticles));
    actions_->Register(starField_.get());
}

Video::~Video() {
    // Nop.
}

void Video::SetViewPort(int width, int height) {
    data_->projection->SetViewPort((float)width, (float)height);
}

void Video::PrepareFrame(const ControlsState &controlsState) {
    if (!data_->focusLander.IsNone()) {
        eventLoop_->Post(ControlsEvent(data_->focusLander, controlsState));
    }
}

void Video::Handle(const ActorCreationEvent &event) {
    Actor *actor = nullptr;
    int   storageId;
    switch (event.actorType) {
        case LanderActor:
            actor = landers_.Get(&storageId);
            break;
        case ShotActor:
            actor = shots_.Get(&storageId);
            break;
        default:
            break;
    }
    
    if (actor) {
        actor->SetData(data_);
        actor->Handle(event);
        actorMap_.Register(event.actor, ActorInfo<Actor>(event.actorType, storageId, actor));
    }
}

void Video::Handle(const ActorTerminationEvent &event) {
    ActorInfo<Actor> *info = actorMap_.Get(event.actor);
    if (info) {
        info->actor()->Handle(event);
        
        switch (info->type()) {
            case LanderActor:
                landers_.Put(info->storageId());
                break;
            case ShotActor:
                shots_.Put(info->storageId());
                break;
            default:
                assert(false);
                break;
        }
        // Don't use info->actor() below.
        
        actorMap_.Unregister(event.actor);
    }
}

void Video::Handle(const MoveEvent &event) {
    ActorInfo<Actor> *info = actorMap_.Get(event.actor);
    if (info) {
        info->actor()->Handle(event);
    }
}

void Video::Handle(const VelocityEvent &event) {
    ActorInfo<Actor> *info = actorMap_.Get(event.actor);
    if (info) {
        info->actor()->Handle(event);
    }
}

void Video::Handle(const AccelerationEvent &event) {
    ActorInfo<Actor> *info = actorMap_.Get(event.actor);
    if (info) {
        info->actor()->Handle(event);
    }
}

void Video::Handle(const FrameGeneratedEvent &event) {
    auto now = steady_clock::now();
    int milliSeconds = (int)duration_cast<milliseconds>(now - lastFrameTime_).count();
    lastFrameTime_ = now;
    data_->frameDeltaTimeS = (float)milliSeconds / 1000.0f;
    
    actions_->Execute();
    
    float observerX, observerZ;
    data_->terrainRenderer->GetObserverPosition(&observerX, &observerZ);
    auto iter = data_->shotParticles->GetIterator();
    while (Particles::ParticleInfo *particle = iter.Next()) {
        data_->mapParameters->xRange.ExpandModuloForObserver(observerX, &particle->position.x);
        data_->mapParameters->zRange.ExpandModuloForObserver(observerZ, &particle->position.z);
    }
    
    data_->projection->Render(0);
    eventLoop_->Post(FrameGeneratedEvent());
}

}    // Namespace Video.
}    // Namespace Zarch.
}    // Namespace kxm.

