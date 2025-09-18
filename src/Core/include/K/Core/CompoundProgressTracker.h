/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_COMPOUNDPROGRESSTRACKER_H_
#define K_CORE_COMPOUNDPROGRESSTRACKER_H_

#include <unordered_map>

#include <K/Core/Ids.h>
#include <K/Core/CompoundProgressTrackerInterface.h>
#include <K/Core/ProgressTrackerCore.h>

namespace K {
namespace Core {

//! Tracks progress of a compound activity, where the subactivities are tracked by child progress trackers.
class CompoundProgressTracker : public ProgressTrackerCore,
                                public virtual CompoundProgressTrackerInterface {
  public:
    CompoundProgressTracker(const std::shared_ptr<HandlerInterface> &handler,
                            const std::shared_ptr<CompoundProgressTrackerInterface> &superActivityProgressTracker);
    CompoundProgressTracker(const CompoundProgressTracker &other)            = delete;
    CompoundProgressTracker &operator=(const CompoundProgressTracker &other) = delete;
    CompoundProgressTracker(CompoundProgressTracker &&other)                 = delete;
    CompoundProgressTracker &operator=(CompoundProgressTracker &&other)      = delete;
    ~CompoundProgressTracker()                                               = default;

    // CompoundProgressTrackerInterface...
    int RegisterSubActivity() override;
    void OnSubActivityProgress(int activity, int percent) override;
    void UnregisterSubActivity(int activity) override;

  private:
    struct SubActivityInfo {
        int percent;

        SubActivityInfo() : percent{0} {}
        // Default copy/move, ok.
    };

    void ComputePercent();

    Ids                                      ids_;
    std::unordered_map<int, SubActivityInfo> subActivities_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_COMPOUNDPROGRESSTRACKER_H_