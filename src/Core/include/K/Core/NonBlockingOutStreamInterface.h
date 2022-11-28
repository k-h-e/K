////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_NONBLOCKINGOUTSTREAMINTERFACE_H_
#define K_CORE_NONBLOCKINGOUTSTREAMINTERFACE_H_

#include <K/Core/OutStreamInterface.h>

namespace K {
namespace Core {

//! Interface to nonblocking output streams.
class NonBlockingOutStreamInterface : public virtual OutStreamInterface {
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

#endif    // K_CORE_NONBLOCKINGOUTSTREAMINTERFACE_H_
