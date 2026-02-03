/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BYTESPANINTERFACE_H_
#define K_CORE_BYTESPANINTERFACE_H_

#include <K/Core/ReadableByteSpanInterface.h>

namespace K {
namespace Core {

//! Interface to entities giving access to a span of bytes.
class ByteSpanInterface : public virtual ReadableByteSpanInterface {
  public:
    //! Grants read/write access to the byte span.
    virtual void *ByteSpanStart() = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BYTESPANINTERFACE_H_
