/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_HANDLEBASE_H_
#define K_CORE_HANDLEBASE_H_

#include <memory>

#include <K/Core/Interface.h>

namespace K {
    namespace Core {
        class ReferenceCountTrackerInterface;
    }
}

namespace K {
namespace Core {

//! Handlee-type-agnostic base class to SharedHandle<T> and UniqueHandle<T>, used to minimize template-induced code size
//! expansion.
class HandleBase : public virtual Interface {
  public:
    //! If the handle is still pointing to a handlee resource, it unregisters from it and becomes a null-handle.
    void Reset();

    //! Tells whether the handle is currently a null-handle.
    bool Null() const;

  protected:
    HandleBase(void *handlee, ReferenceCountTrackerInterface *referenceCountTracker,
               const std::shared_ptr<Interface> &keepAlive);
    HandleBase() = delete;
    HandleBase(const HandleBase &other);
    HandleBase &operator=(const HandleBase &other);
    HandleBase(HandleBase &&other);
    HandleBase &operator=(HandleBase &&other);
    ~HandleBase();

    void                           *handlee_;
    ReferenceCountTrackerInterface *referenceCountTracker_;
    std::shared_ptr<Interface>     keepAlive_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_HANDLEBASE_H_
