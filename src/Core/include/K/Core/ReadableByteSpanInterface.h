/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_READABLEBYTESPANINTERFACE_H_
#define K_CORE_READABLEBYTESPANINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to entities giving read-only access to a span of bytes.
class ReadableByteSpanInterface : public virtual Interface {
  public:
    //! Grants read-only access to the byte span.
    virtual const void *ByteSpanStartReadOnly() = 0;
    //! Tells the size in bytes of the byte span, which is guaranteed to be positive.
    virtual int ByteSpanSize() const = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_READABLEBYTESPANINTERFACE_H_
