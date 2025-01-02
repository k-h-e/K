/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_IOBUFFERINTERFACE_H_
#define K_CORE_IOBUFFERINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to I/O buffers.
class IoBufferInterface : public virtual Interface {
  public:
    //! Grants access to the buffer content.
    virtual void *Content() = 0;
    //! Tells the buffer's size in bytes (guaranteed to be > 0).
    virtual int Size() const = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_IOBUFFERINTERFACE_H_
