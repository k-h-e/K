/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/SimpleProgressTracker.h>

#include <K/Core/NumberTools.h>

using std::shared_ptr;
using std::string;

namespace K {
namespace Core {

SimpleProgressTracker::SimpleProgressTracker(const shared_ptr<HandlerInterface> &handler, const string &activity)
        : ProgressTracker{handler, activity},
          activityNumSteps_{1},
          currentNumSteps_{0} {
    // Nop.
}

void SimpleProgressTracker::SetNumSteps(int numSteps) {
    activityNumSteps_ = numSteps;
    NumberTools::ClampMin(activityNumSteps_, 1);
    NumberTools::ClampMax(currentNumSteps_, activityNumSteps_);
}

void SimpleProgressTracker::OnStep() {
    if (currentNumSteps_ < activityNumSteps_) {
        ++currentNumSteps_;
        if (currentNumSteps_ < activityNumSteps_) {
            int progress { static_cast<int>(static_cast<float>(currentNumSteps_)/static_cast<float>(activityNumSteps_)
                                                * 100.0f + .5f) };
            NumberTools::Clamp(progress, 0, 99);
            SetProgress(progress);
        } else {
            SetProgress(100);
        }
    }
}

}    // Namespace Core.
}    // Namespace K.
