////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BLOCKINGOUTSTREAMINTERFACE_H_
#define K_CORE_BLOCKINGOUTSTREAMINTERFACE_H_

#include <K/Core/OutStreamInterface.h>

namespace K {
namespace Core {

//! Interface to blocking output streams.
class BlockingOutStreamInterface : public virtual OutStreamInterface {
  public:
    //! Writes the specified data to the stream with blocking semantics.
    /*!
     *  \return
     *  Number of bytes actually written. Might be less than the data size (short write). <c>0</c> means that the write
     *  operation has failed and that the stream will be in <c>ErrorState()</c>.
     */
    virtual int WriteBlocking(const void *data, int dataSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGOUTSTREAMINTERFACE_H_
