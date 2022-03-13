////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BLOCKINGWRITEINTERFACE_H_
#define K_CORE_BLOCKINGWRITEINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to data writers providing blocking writing.
class BlockingWriteInterface : public virtual Interface {
  public:
    //! Writes the specified data to the stream with blocking semantics.
    /*!
     *  \return
     *  Number of bytes actually written. Might be less than the data size (short write). <c>0</c> means that the write
     *  operation has failed.
     */
    virtual int WriteBlocking(const void *data, int dataSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGWRITEINTERFACE_H_
