/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_COMPOUNDPROGRESSTRACKER_H_
#define K_CORE_COMPOUNDPROGRESSTRACKER_H_

#include <vector>

#include <K/Core/NumberTools.h>
#include <K/Core/ProgressTracker.h>

namespace K {
namespace Core {

//! Keeps track of a compound activity's progress in percent.
class CompoundProgressTracker : public ProgressTracker,
                                private virtual ProgressTracker::ParentInterface {
  public:
    CompoundProgressTracker(const std::shared_ptr<HandlerInterface> &handler, const std::string &activity);
    CompoundProgressTracker()                                                    = delete;
    CompoundProgressTracker(const CompoundProgressTracker &other)                = delete;
    CompoundProgressTracker &operator=(const CompoundProgressTracker &other)     = delete;
    CompoundProgressTracker(CompoundProgressTracker &&other)                     = delete;
    CompoundProgressTracker &operator=(CompoundProgressTracker &&other)          = delete;
    ~CompoundProgressTracker();

    //! Adds the specified child activity tracker.
    void AddChild(const std::shared_ptr<ProgressTracker> &child);
    //! Adds the specified child activity tracker.
    void AddChild(const std::shared_ptr<ProgressTracker> &child, float weight);

  private:
    struct ChildInfo {
        std::shared_ptr<ProgressTracker> child;
        int                              progress;
        float                            weight;

        ChildInfo(const std::shared_ptr<ProgressTracker> &aChild, float aWeight)
            : child{aChild}, progress{0}, weight{aWeight} { NumberTools::ClampMin(weight, .1f); }
        ChildInfo() : progress{0}, weight{1.0f} {}
        ChildInfo(const ChildInfo &other)            = default;
        ChildInfo &operator=(const ChildInfo &other) = default;
        ChildInfo(ChildInfo &&other)                 = default;
        ChildInfo &operator=(ChildInfo &&other)      = default;
    };

    // ParentInterface...
    void OnProgressUpdate(int id, int progressPercent) override;

    std::vector<ChildInfo> children_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_COMPOUNDPROGRESSTRACKER_H_
