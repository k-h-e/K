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

#include <K/Core/ReadFailedInterface.h>

namespace K {
namespace Core {

//! Interface to data readers providing blocking reading.
class BlockingReadInterface : public virtual ReadFailedInterface {
  public:
    //! Reads data into the specified buffer with blocking semantics.
    /*!
     *  In case the read operation fails, read failed state will be set (see <c>ReadFailedInterface::ReadFailed()</c>).
     *  If read failed state has already been set when the method gets called, the read will not execute and the method
     *  reports failure immediately.
     *
     *  \return
     *  Number of bytes actually read. Will not be larger than the buffer size. Might be less than the buffer size
     *  (short read). <c>0</c> means that the read operation has failed, including cases where reads are currently
     *  disabled as a result of an earlier read failure. In case of failure, read failed state will be set, see
     *  <c>ReadFailedInterface::ReadFailed()</c>.
     */
    virtual int ReadBlocking(void *buffer, int bufferSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGREADINTERFACE_H_
