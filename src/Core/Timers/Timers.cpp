////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/Timers.h>

#include <K/Core/ThreadPool.h>
#include "SharedState.h"
#include "Worker.h"

using std::make_shared;
using std::shared_ptr;
using std::chrono::milliseconds;

namespace K {
namespace Core {

Timers::Timers(const shared_ptr<ThreadPool> &threadPool)
        : threadPool_(threadPool) {
    shared_ = make_shared<SharedState>();
    worker_ = make_shared<Worker>(shared_);

    threadPool_->Run(worker_.get(), shared_.get(), 0);
}

Timers::~Timers() {
    shared_->RequestShutDown();
    shared_->WaitForWorkerFinished();
}

int Timers::AddTimer(milliseconds interval, TimerHandlerInterface *handler) {
    return shared_->AddTimer(interval, handler);
}

void Timers::RemoveTimer(int timer) {
    shared_->RemoveTimer(timer);
}

}    // Namespace Core.
}    // Namespace K.
