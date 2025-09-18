/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/ProgressTrackerCore.h>

#include <K/Core/CompoundProgressTrackerInterface.h>
#include <K/Core/NumberTools.h>

using std::shared_ptr;
using K::Core::NumberTools;

namespace K {
namespace Core {

ProgressTrackerCore::ProgressTrackerCore(
    const shared_ptr<HandlerInterface> &handler,
    const shared_ptr<CompoundProgressTrackerInterface> &superActivityProgressTracker)
        : handler_{handler},
          superActivityProgressTracker_{superActivityProgressTracker},
          subActivityId_{0},
          percent_{-1} {
    if (superActivityProgressTracker_) {
        subActivityId_ = superActivityProgressTracker_->RegisterSubActivity();
    }
}

ProgressTrackerCore::~ProgressTrackerCore() {
    if (superActivityProgressTracker_) {
        superActivityProgressTracker_->UnregisterSubActivity(subActivityId_);
    }
}

// ---

void ProgressTrackerCore::OnPercentComputed(int percent) {
    NumberTools::Clamp(percent, 0, 100);
    if (percent > percent_) {
        percent_ = percent;
        if (superActivityProgressTracker_) {
            superActivityProgressTracker_->OnSubActivityProgress(subActivityId_, percent_);
        }
        if (handler_) {
            handler_->OnProgress(percent_);
        }
    }
}

}    // Namespace Core.
}    // Namespace K.
