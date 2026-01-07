/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/CompoundProgressTracker.h>

using std::shared_ptr;
using std::string;

namespace K {
namespace Core {

CompoundProgressTracker::CompoundProgressTracker(const shared_ptr<HandlerInterface> &handler, const string &activity)
        : ProgressTracker{handler, activity} {
    // Nop.
}

CompoundProgressTracker::~CompoundProgressTracker() {
    for (ChildInfo &info : children_) {
        info.child->SetParent(nullptr, 0);
    }
}  

void CompoundProgressTracker::AddChild(const shared_ptr<ProgressTracker> &child) {
    AddChild(child, 1.0f);
}

void CompoundProgressTracker::AddChild(const shared_ptr<ProgressTracker> &child, float weight) {
    int id { static_cast<int>(children_.size()) };
    child->SetParent(this, id);
    children_.push_back(ChildInfo{child, weight});
}

// ---

void CompoundProgressTracker::OnProgressUpdate(int id, int progressPercent) {
    if ((id >= 0) && (id < static_cast<int>(children_.size()))) {    // At least one iteration.
        ChildInfo &info { children_[id] };
        info.progress = progressPercent;
        NumberTools::Clamp(info.progress, 0, 100);

        float totalProgress { 0.0f };
        float totalWeight   { 0.0f };
        bool  allDone       { true };
        for (ChildInfo &current : children_) {
            if (current.progress < 100) {
                allDone = false;
            }
            totalProgress += current.weight * static_cast<float>(current.progress);
            totalWeight   += current.weight;
        }
        totalProgress /= totalWeight;

        if (!allDone) {
            int progress { static_cast<int>(totalProgress + .5f) };
            NumberTools::Clamp(progress, 0, 99);
            SetProgress(progress);
        } else {
            SetProgress(100);
        }
    }
}  

}    // Namespace Core.
}    // Namespace K.
