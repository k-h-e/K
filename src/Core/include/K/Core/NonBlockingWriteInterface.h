////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_NONBLOCKINGWRITEINTERFACE_H_
#define K_CORE_NONBLOCKINGWRITEINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to data writers providing nonblocking writing.
class NonBlockingWriteInterface : public virtual Interface {
  public:
    //! Writes the specified data to the stream with nonblocking semantics.
    /*!
     *  \return
     *  Number of bytes actually written. Might be less than the data size (short write). <c>0</c> means that no data
     *  could be accepted for writing.
     */
    virtual int WriteNonBlocking(const void *data, int dataSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_NONBLOCKINGWRITEINTERFACE_H_
