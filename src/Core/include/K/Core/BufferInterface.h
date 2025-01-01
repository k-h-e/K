/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BUFFERINTERFACE_H_
#define K_CORE_BUFFERINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to buffers.
class BufferInterface : public virtual Interface {
  public:
    //! Grants read access to the buffered data.
    virtual const void *Data() const = 0;
    //! Tells the size of the buffered data in bytes.
    virtual int DataSize() const = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BUFFERINTERFACE_H_
