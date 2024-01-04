/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_NONBLOCKINGINSTREAMINTERFACE_H_
#define K_CORE_NONBLOCKINGINSTREAMINTERFACE_H_

#include <K/Core/InStreamInterface.h>

namespace K {
namespace Core {

//! Interface to nonblocking input streams.
class NonBlockingInStreamInterface : public virtual InStreamInterface {
  public:
    //! Reads data into the specified buffer with nonblocking semantics.
    /*!
     *  \return
     *  Number of bytes actually read. Will not be larger than the buffer size. Might be less than the buffer size
     *  (short read). <c>0</c> means that no data was available for reading.
     */
    virtual int ReadNonBlocking(void *buffer, int bufferSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_NONBLOCKINGINSTREAMINTERFACE_H_
