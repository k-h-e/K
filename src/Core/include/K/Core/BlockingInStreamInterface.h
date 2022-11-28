////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BLOCKINGINSTREAMINTERFACE_H_
#define K_CORE_BLOCKINGINSTREAMINTERFACE_H_

#include <K/Core/InStreamInterface.h>

namespace K {
namespace Core {

//! Interface to blocking input streams.
class BlockingInStreamInterface : public virtual InStreamInterface {
  public:
    //! Reads data into the specified buffer with blocking semantics.
    /*!
     *  \return
     *  Number of bytes actually read. Will not be larger than the buffer size. Might be less than the buffer size
     *  (short read). <c>0</c> means that the read operation has failed and that the stream will be in
     *  <c>ErrorState()</c>.
     */
    virtual int ReadBlocking(void *buffer, int bufferSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGINSTREAMINTERFACE_H_
