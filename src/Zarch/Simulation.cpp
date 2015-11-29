//
//  Simulation.cpp
//  kxm
//
//  Created by Kai Hergenröther on 4/7/13.
//
//


#include <Zarch/Simulation.h>

#include <kxm/Core/logging.h>
#include <Zarch/Zarch.h>
#include <Zarch/GameLogic/GameLogic.h>
#include <Zarch/Physics/Physics.h>
#include <Zarch/Events/FrameTimeEvent.h>


using namespace std;
using namespace std::chrono;
using namespace kxm::Core;
using namespace kxm::Game;


namespace kxm {
namespace Zarch {

Simulation::Simulation(const shared_ptr<EventQueueHub> &eventQueueHub)
        : eventQueue_(new EventQueue(EventQueueHub::initialBufferSize)),
          processes_(new Processes<ZarchProcess::ProcessType>()),
          eventQueueHub_(eventQueueHub),
          lastFrameTime_(steady_clock::now()) {
    Zarch::RegisterEvents(eventQueue_.get());
    hubClientId_ = eventQueueHub_->AllocUniqueClientId();
        
    physics_   = shared_ptr<Physics>(new Physics(eventQueue_, processes_));
    gameLogic_ = shared_ptr<GameLogic>(new GameLogic(eventQueue_, processes_));
}

void Simulation::ExecuteAction() {
    puts("simulation thread spawned");
    
    bool shutdownRequested = false;
    while (!shutdownRequested) {
        shutdownRequested = !eventQueue_->SyncWithHub(eventQueueHub_.get(), hubClientId_, true);
        GenerateTimeEvent();
        eventQueue_->ProcessEvents();
        
        processes_->ExecuteProcesses();
    }
    
    puts("simulation thread terminating");
}

void Simulation::GenerateTimeEvent() {
    auto now = steady_clock::now();
    int milliSeconds = (int)duration_cast<milliseconds>(now - lastFrameTime_).count();
    lastFrameTime_ = now;
    float frameDeltaTimeS = (float)milliSeconds / 1000.0f;
    eventQueue_->Schedule(FrameTimeEvent(frameDeltaTimeS));
}

}
}

