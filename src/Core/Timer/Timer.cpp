/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/Timer.h>
#include "Adapter.h"
#include "LoopThreadState.h"

using std::make_unique;
using std::shared_ptr;
using std::chrono::milliseconds;
using K::Core::Timers;

namespace K {
namespace Core {

Timer::Timer(milliseconds interval, const shared_ptr<RunLoop> &runLoop, const shared_ptr<Timers> &timers)
        : loopThreadState_(make_unique<LoopThreadState>(runLoop, timers)) {
    loopThreadState_->runLoopClientId = loopThreadState_->runLoop->AddClient(loopThreadState_.get());
    adapter_ = make_unique<Adapter>(loopThreadState_->runLoop, loopThreadState_->runLoopClientId);
    loopThreadState_->timer = loopThreadState_->timers->AddTimer(interval, adapter_.get(), false);
}

Timer::~Timer() {
    loopThreadState_->timers->RemoveTimer(loopThreadState_->timer);
    loopThreadState_->runLoop->RemoveClient(loopThreadState_->runLoopClientId);
}

void Timer::Register(HandlerInterface *handler) {
    loopThreadState_->handler = handler;
}

}    // Namespace Core.
}    // Namespace K.
