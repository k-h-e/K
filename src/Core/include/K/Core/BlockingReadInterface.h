////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BLOCKINGREADINTERFACE_H_
#define K_CORE_BLOCKINGREADINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to data readers providing blocking reading.
class BlockingReadInterface : public virtual Interface {
  public:
    //! Reads data into the specified buffer with blocking semantics.
    /*!
     *  \return
     *  Number of bytes actually read. Will not be larger than the buffer size. Might be less than the buffer size
     *  (short read). <c>0</c> means that the read operation has failed.
     */
    virtual int ReadBlocking(void *buffer, int bufferSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGREADINTERFACE_H_
