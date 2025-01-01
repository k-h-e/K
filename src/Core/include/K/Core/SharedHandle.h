/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SHAREDHANDLE_H_
#define K_CORE_SHAREDHANDLE_H_

#include <K/Core/HandleBase.h>

namespace K {
namespace Core {

//! Object handle with shared semantics, allowing for reference counted resource management the RAII way.
/*!
 *  Similar to a shared smart pointer, yet generalized to include use cases other than those concerning dynamic memory:
 *  when a handle is copied, it simply tells its handlee that there now is one more handle to it. Likewise, when a
 *  handle dies because it goes out of scope, or when it is set explicitly to be a null-handle, it tells its handlee
 *  that there is now one handle less to it. Actual management of the handled resource's state is implemented by the
 *  handlee, that typically is a private class of some resource providing component.
 */
template<class T>
class SharedHandle : public HandleBase {
  public:
    //! Constructs a null-handle, i.e. a handle not handling any handlee resource.
    SharedHandle() : HandleBase(nullptr, nullptr) {}

    //! Constructs a handle handling the specified resource.
    SharedHandle(T *handlee, ReferenceCountTrackerInterface *referenceCountTracker,
                 const std::shared_ptr<Interface> &keepAlive)
        : HandleBase{handlee, referenceCountTracker, keepAlive} {}
    
    SharedHandle(const SharedHandle &other)            = default;
    SharedHandle &operator=(const SharedHandle &other) = default;
    SharedHandle(SharedHandle &&other)                 = default;
    SharedHandle &operator=(SharedHandle &&other)      = default;
    ~SharedHandle()                                    = default;

    //! Returns <c>false</c> in case the handle is a null-handle, i.e. in case it is not (or no longer) handling a
    //! handlee resource.
    explicit operator bool() const {
        return !isNull();
    }

    //! Allows the handle to be dereferenced just like a regular pointer.
    T *operator->() const {
        assert(handlee_ != nullptr);
        return get();
    }

    //! Allows the handle to be dereferenced just like a regular pointer.
    T &operator*() const {
        assert(handlee_ != nullptr);
        return *get();
    }

    //! Returns a dumb pointer to the handlee.
    T *get() const {
        return static_cast<T *>(handlee_);
    }
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SHAREDHANDLE_H_
