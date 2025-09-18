/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/ProgressTracker.h>

#include <K/Core/NumberTools.h>

using std::shared_ptr;
using K::Core::NumberTools;

namespace K {
namespace Core {

ProgressTracker::ProgressTracker(const shared_ptr<HandlerInterface> &handler,
                                 const shared_ptr<CompoundProgressTrackerInterface> &superActivityProgressTracker)
        : ProgressTrackerCore{handler, superActivityProgressTracker},
          numSteps_{100},
          numStepsCompleted_{0} {
    // Nop.
}

void ProgressTracker::SetNumSteps(int numSteps) {
    if (numSteps > 0) {
        numSteps_ = numSteps;
        NumberTools::Clamp(numStepsCompleted_, 0, numSteps_);
        ComputePercent();
    }
}

void ProgressTracker::OnStepCompleted() {
    if (numStepsCompleted_ < numSteps_) {
        ++numStepsCompleted_;
        ComputePercent();
    }
}

// ---

void ProgressTracker::ComputePercent() {
    int percent { static_cast<int>(100.0f*static_cast<float>(numStepsCompleted_)/static_cast<float>(numSteps_) + .5f) };
    OnPercentComputed(percent);
}

}    // Namespace Core.
}    // Namespace K.
