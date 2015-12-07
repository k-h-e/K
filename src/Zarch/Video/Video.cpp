#include <Zarch/Video/Video.h>

#include <kxm/Core/logging.h>
#include <Vectoid/PerspectiveProjection.h>
#include <Vectoid/Camera.h>
#include <Vectoid/CoordSys.h>
#include <Vectoid/Geode.h>
#include <Vectoid/Particles.h>
#include <Vectoid/AgeColoredParticles.h>
#include <Vectoid/ParticlesRenderer.h>
#include <Vectoid/Transform.h>
#include <Game/EventQueueClientInterface.h>
#include <Game/ProcessesClientInterface.h>
#include <Zarch/Video/CameraProcess.h>
#include <Zarch/Video/StarFieldProcess.h>
#include <Zarch/Video/ThrusterParticlesProcess.h>
#include <Zarch/Video/TerrainRenderer.h>
#include <Zarch/LanderGeometry.h>
#include <Zarch/MapParameters.h>
#include <Zarch/Terrain.h>
#include <Zarch/Events/ZarchEvent.h>
#include <Zarch/Events/FrameTimeEvent.h>
#include <Zarch/Events/LanderMoveEvent.h>
#include <Zarch/Events/LanderVelocityEvent.h>
#include <Zarch/Events/LanderThrusterEvent.h>


using namespace std;
using namespace kxm::Vectoid;
using namespace kxm::Game;


namespace kxm {
namespace Zarch {

Video::Video(shared_ptr<EventQueueClientInterface> eventQueue,
             shared_ptr<ProcessesClientInterface> processes)
        : eventQueue_(eventQueue),
          processes_(processes) {
    data_ = shared_ptr<Data>(new Data());
    data_->mapParameters = shared_ptr<MapParameters>(new MapParameters());
    data_->terrain       = shared_ptr<Terrain>(new Terrain(data_->mapParameters));
    
    // Install scene graph...
    data_->projection = shared_ptr<PerspectiveProjection>(new PerspectiveProjection());
    data_->projection->SetWindowSize(11.0f);
    data_->projection->SetViewingDepth(11.0f);
    data_->projection->SetEyepointDistance(11.0f);
    data_->camera = shared_ptr<Camera>(new Camera());
    data_->projection->AddChild(data_->camera);
    data_->landerCoordSys = shared_ptr<CoordSys>(new CoordSys());
    data_->camera->AddChild(data_->landerCoordSys);
    shared_ptr<LanderGeometry> landerGeometry(new LanderGeometry());
    data_->landerCoordSys->AddChild(shared_ptr<Geode>(new Geode(landerGeometry)));
    data_->terrainRenderer = shared_ptr<TerrainRenderer>(new TerrainRenderer(data_->terrain,
                                                                             data_->mapParameters));
    data_->camera->AddChild(shared_ptr<Geode>(new Geode(data_->terrainRenderer)));
    
    shared_ptr<Particles> thrusterParticles(new Particles()),
                          shotsParticles(new Particles()),
                          starFieldParticles(new Particles());
    data_->camera->AddChild(shared_ptr<Geode>(new Geode(shared_ptr<AgeColoredParticles>(
        new AgeColoredParticles(thrusterParticles)))));
    data_->camera->AddChild(shared_ptr<Geode>(new Geode(shared_ptr<ParticlesRenderer>(
        new ParticlesRenderer(shotsParticles)))));
    data_->camera->AddChild(shared_ptr<Geode>(new Geode(shared_ptr<ParticlesRenderer>(
        new ParticlesRenderer(starFieldParticles)))));
    
    cameraProcess_ = unique_ptr<CameraProcess>(new CameraProcess(data_));
    processes_->RegisterProcess(cameraProcess_.get(), this);
    
    starFieldProcess_ = unique_ptr<StarFieldProcess>(new StarFieldProcess(data_,
                                                                          starFieldParticles));
    processes_->RegisterProcess(starFieldProcess_.get(), this);
    
    thrusterParticlesProcess_ = unique_ptr<ThrusterParticlesProcess>(new ThrusterParticlesProcess(
                                                                         data_,
                                                                         thrusterParticles));
    processes_->RegisterProcess(thrusterParticlesProcess_.get(), this);
}

Video::~Video() {
    // Nop.
}

void Video::SetViewPort(int width, int height) {
    data_->projection->SetViewPort((float)width, (float)height);
}

void Video::RenderFrame() {
    data_->projection->Render(0);
}

vector<Event::EventType> Video::EnumerateHandledEvents() {
    return vector<Event::EventType>{ FrameTimeEvent::type,
                                     LanderMoveEvent::type,
                                     LanderVelocityEvent::type,
                                     LanderThrusterEvent::type  };
}

void Video::HandleProcessFinished(Game::ProcessInterface *process) {
    // Nop.
}

void Video::HandleEvent(const Game::Event &event) {
    static_cast<const Zarch::ZarchEvent &>(event).DispatchToVideo(this);
}

void Video::HandleFrameTimeEvent(const FrameTimeEvent &event) {
    data_->frameDeltaTimeS = event.timeS;
}

void Video::HandleLanderMoveEvent(const LanderMoveEvent &event) {
    data_->landerCoordSys->SetTransform(event.newLanderTransform);
    
    Vector landerPosition = data_->landerCoordSys->Position();
    data_->terrainRenderer->SetObserverPosition(landerPosition.x, landerPosition.z);
}

void Video::HandleLanderVelocityEvent(const LanderVelocityEvent &event) {
    data_->landerVelocity = event.velocity;
}

void Video::HandleLanderThrusterEvent(const LanderThrusterEvent &event) {
    data_->landerThrusterEnabled = event.thrusterEnabled;
}

}    // Namespace Zarch.
}    // Namespace kxm.

