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

#include <K/Core/WriteFailedInterface.h>

namespace K {
namespace Core {

//! Interface to data writers providing nonblocking writing.
class NonBlockingWriteInterface : public virtual WriteFailedInterface {
  public:
    //! Writes the specified data to the stream with nonblocking semantics.
    /*!
     *  In case the write operation fails, write failed state will be set
     *  (see <c>WriteFailedInterface::WriteFailed()</c>). If write failed state has already been set when the method
     *  gets called, the write will not execute and the method reports failure immediately.
     *
     *  \return
     *  Number of bytes actually written. Might be less than the data size (short write). <c>0</c> means either that
     *  simply no more data can currently be accepted for writing, or that the write operation has failed, including
     *  cases where writes are currently disabled as a result of a previous write failure. In case of failure, write
     *  failed state will be set, see <c>WriteFailedInterface::WriteFailed()</c>.
     */
    virtual int WriteNonBlocking(const void *data, int dataSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_NONBLOCKINGWRITEINTERFACE_H_
