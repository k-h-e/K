/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/HandleBase.h>

#include <cassert>
#include <utility>

#include <K/Core/ReferenceCountTrackerInterface.h>

using std::shared_ptr;

namespace K {
namespace Core {

HandleBase::HandleBase(void *handlee, ReferenceCountTrackerInterface *referenceCountTracker,
                       const shared_ptr<Interface> &keepAlive)
        : handlee_{handlee},
          referenceCountTracker_{nullptr} {
    if (handlee_ != nullptr) {
        referenceCountTracker_ = referenceCountTracker;
        keepAlive_             = keepAlive;

        assert(referenceCountTracker_ != nullptr);
        referenceCountTracker->OnReferenceAdded();
    }
}

HandleBase::HandleBase(const HandleBase &other)
        : handlee_{nullptr},
          referenceCountTracker_{nullptr} {
    *this = other;
}

HandleBase &HandleBase::operator=(const HandleBase &other) {
    if (&other != this) {
        if (handlee_ != nullptr) {
            referenceCountTracker_->OnReferenceRemoved();
        }

        handlee_               = other.handlee_;
        referenceCountTracker_ = other.referenceCountTracker_;
        keepAlive_             = other.keepAlive_;

        if (handlee_ != nullptr) {
            referenceCountTracker_->OnReferenceAdded();
        }
    }

    return *this;
}

HandleBase::HandleBase(HandleBase &&other)
        : handlee_{nullptr},
          referenceCountTracker_{nullptr} {
    *this = std::move(other);
}

HandleBase &HandleBase::operator=(HandleBase &&other) {
    handlee_               = other.handlee_;
    referenceCountTracker_ = other.referenceCountTracker_;
    keepAlive_             = std::move(other.keepAlive_);

    other.handlee_               = nullptr;
    other.referenceCountTracker_ = nullptr;
    other.keepAlive_.reset();

    return *this;
}

HandleBase::~HandleBase() {
    Reset();
}

void HandleBase::Reset() {
    if (handlee_ != nullptr) {
        referenceCountTracker_->OnReferenceRemoved();
        handlee_               = nullptr;
        referenceCountTracker_ = nullptr;
        keepAlive_.reset();
    }
}

bool HandleBase::Null() const {
    return (handlee_ == nullptr);
}

}    // Namespace Core.
}    // Namespace K.
