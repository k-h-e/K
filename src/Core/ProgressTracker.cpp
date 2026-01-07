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
using std::string;

namespace K {
namespace Core {

ProgressTracker::ProgressTracker(const shared_ptr<HandlerInterface> &handler, const string &activity)
        : handler_{handler},
          activity_{activity},
          parent_{nullptr},
          idForParent_{0},
          progress_{-1},
          oldProgress_{-1} {
    // Nop.
}

ProgressTracker::~ProgressTracker() {
    progress_ = 100;
    NotifyHandler();
}

void ProgressTracker::SetParent(ParentInterface *parent, int idForParent) {
    parent_      = parent;
    idForParent_ = (parent_ != nullptr) ? idForParent : 0;
    oldProgress_ = -1;
}

// ---

void ProgressTracker::SetProgress(int progressPercent) {
    progress_ = progressPercent;
    NumberTools::Clamp(progress_, 0, 100);
    if (progress_ != oldProgress_) {
        oldProgress_ = progress_;
        NotifyParent();
        NotifyHandler();
    }
}

void ProgressTracker::NotifyParent() {
    if (parent_ != nullptr) {
        parent_->OnProgressUpdate(idForParent_, progress_);
    }
}

void ProgressTracker::NotifyHandler() {
    if (handler_) {
        handler_->OnProgressUpdate(activity_, progress_);
    }
}

}    // Namespace Core.
}    // Namespace K.
