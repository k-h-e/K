/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_PROGRESSTRACKER_H_
#define K_CORE_PROGRESSTRACKER_H_

#include <K/Core/ProgressTrackerCore.h>

namespace K {
namespace Core {

//! Tracks progress of an activity that has a given number of discrete steps (of comparable complexity).
class ProgressTracker : public ProgressTrackerCore {
  public:
    ProgressTracker(const std::shared_ptr<HandlerInterface> &handler,
                    const std::shared_ptr<CompoundProgressTrackerInterface> &superActivityProgressTracker);
    ProgressTracker(const ProgressTracker &other)            = delete;
    ProgressTracker &operator=(const ProgressTracker &other) = delete;
    ProgressTracker(ProgressTracker &&other)                 = delete;
    ProgressTracker &operator=(ProgressTracker &&other)      = delete;
    ~ProgressTracker()                                       = default;

    //! Sets the number of total steps.
    void SetNumSteps(int numSteps);
    //! Tells the tracker that another activity step has been completed.
    void OnStepCompleted();

  private:
    void ComputePercent();

    int numSteps_;
    int numStepsCompleted_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_PROGRESSTRACKER_H_
