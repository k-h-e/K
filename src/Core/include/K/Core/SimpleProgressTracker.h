/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SIMPLEPROGRESSTRACKER_H_
#define K_CORE_SIMPLEPROGRESSTRACKER_H_

#include <K/Core/ProgressTracker.h>

namespace K {
namespace Core {

//! Keeps track of the progress in percent of a simple activity consisting of a fixed number of steps.
class SimpleProgressTracker : public ProgressTracker {
  public:
    SimpleProgressTracker(const std::shared_ptr<HandlerInterface> &handler, const std::string &activity);
    SimpleProgressTracker()                                                  = delete;
    SimpleProgressTracker(const SimpleProgressTracker &other)                = delete;
    SimpleProgressTracker &operator=(const SimpleProgressTracker &other)     = delete;
    SimpleProgressTracker(SimpleProgressTracker &&other)                     = delete;
    SimpleProgressTracker &operator=(SimpleProgressTracker &&other)          = delete;

    //! Tells the tracker about the total number of steps in the activity.
    void SetNumSteps(int numSteps);
    //! Informs the progress tracker that another one of the respective activity's steps has been performed.
    void OnStep();

  private:
    int activityNumSteps_;
    int currentNumSteps_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SIMPLEPROGRESSTRACKER_H_
