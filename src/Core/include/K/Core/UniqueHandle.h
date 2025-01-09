/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_UNIQUEHANDLE_H_
#define K_CORE_UNIQUEHANDLE_H_

#include <cassert>

#include <K/Core/HandleBase.h>

namespace K {
namespace Core {

//! Object handle with unique semantics, allowing for resource management the RAII way.
/*!
 *  Similar to a unique smart pointer, yet generalized to include use cases other than those concerning dynamic memory:
 *  when a handle is created, it simply tells its handlee that there now exists a reference to it. Likewise, when a
 *  handle dies because it goes out of scope, or when it is set explicitly to be a null-handle, it tells its handlee
 *  that the reference to it is gone. Actual management of the handled resource's state is implemented by the handlee,
 *  that typically is a private class of some resource providing component.
 */
template<class T>
class UniqueHandle : public HandleBase {
  public:
    //! Constructs a null-handle, i.e. a handle not handling any handlee resource.
    UniqueHandle() : HandleBase(nullptr, nullptr, nullptr) {}

    //! Constructs a handle handling the specified resource.
    UniqueHandle(T *handlee, ReferenceCountTrackerInterface *referenceCountTracker,
                 const std::shared_ptr<Interface> &keepAlive)
        : HandleBase{handlee, referenceCountTracker, keepAlive} {}
    
    UniqueHandle(const UniqueHandle &other)            = delete;
    UniqueHandle &operator=(const UniqueHandle &other) = delete;
    UniqueHandle(UniqueHandle &&other)                 = default;
    UniqueHandle &operator=(UniqueHandle &&other)      = default;
    ~UniqueHandle()                                    = default;

    //! Returns <c>false</c> in case the handle is a null-handle, i.e. in case it is not (or no longer) handling a
    //! handlee resource.
    explicit operator bool() const {
        return !Null();
    }

    //! Allows the handle to be dereferenced just like a regular pointer.
    T *operator->() const {
        assert(handlee_ != nullptr);
        return Get();
    }

    //! Allows the handle to be dereferenced just like a regular pointer.
    T &operator*() const {
        assert(handlee_ != nullptr);
        return *Get();
    }

    //! Returns a dumb pointer to the handlee.
    T *Get() const {
        return static_cast<T *>(handlee_);
    }
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_UNIQUEHANDLE_H_
