//
//  Video.cpp
//  kxm
//
//  Created by Kai Hergenröther on 12/3/12.
//
//


#include <kxm/Zarch/Video/Video.h>

#include <kxm/Core/logging.h>
#include <kxm/Zarch/Video/CameraProcess.h>
#include <kxm/Zarch/Video/TerrainProcess.h>
#include <kxm/Zarch/Video/StarFieldProcess.h>
#include <kxm/Zarch/events.h>

using namespace std;
using namespace boost;
using namespace kxm::Vectoid;
using namespace kxm::Game;


namespace kxm {
namespace Zarch {

Video::Video(
        EventQueue<ZarchEvent::EventType> *eventQueue,
        shared_ptr<MapParameters> mapParameters, shared_ptr<CoordSysInterface> camera,
        shared_ptr<TerrainRenderer> terrainRenderer, shared_ptr<Particles> starFieldParticles)
    : landerStateInfo_(new LanderProcess::LanderStateInfo()) {
    shared_ptr<CameraProcess> cameraProcess(
        new CameraProcess(camera, landerStateInfo_, mapParameters));
    processes_.AddProcess(cameraProcess);
    processes_.AddProcess(shared_ptr<Process>(
        new TerrainProcess(terrainRenderer, landerStateInfo_)));
    processes_.AddProcess(shared_ptr<Process>(
        new StarFieldProcess(starFieldParticles, cameraProcess->CameraState(), mapParameters)));
    
    processContext_.eventQueue = eventQueue;
    processContext_.processes  = &processes_;
}

void Video::HandleEvent(const Event &event) {
    switch (static_cast<const ZarchEvent &>(event).Type()) {
        case ZarchEvent::LanderMovedEvent:
            landerStateInfo_->transform
                = static_cast<const TransformEvent &>(event).Transform();
            break;
        default:
            break;
    }
}

void Video::ExecuteProcesses() {
    processes_.ExecuteProcesses(processContext_);
}

}    // Namespace Zarch.
}    // Namespace kxm.