/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/Signal.h>

#include "Adapter.h"
#include "LoopThreadState.h"

using std::make_unique;
using std::shared_ptr;

namespace K {
namespace Core {

Signal::Signal(const shared_ptr<RunLoop> &runLoop)
        : loopThreadState_(make_unique<LoopThreadState>(runLoop)) {
    loopThreadState_->runLoopClientId = loopThreadState_->runLoop->AddClient(loopThreadState_.get());
    adapter_ = make_unique<Adapter>(loopThreadState_->runLoop, loopThreadState_->runLoopClientId);
}

Signal::~Signal() {
    loopThreadState_->runLoop->RemoveClient(loopThreadState_->runLoopClientId);
}

void Signal::Register(HandlerInterface *handler) {
    loopThreadState_->handler = handler;
}

void Signal::Raise() {
    adapter_->Raise();
}

}    // Namespace Core.
}    // Namespace K.
