/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_NONBLOCKINGOUTSTREAMINTERFACE_H_
#define K_CORE_NONBLOCKINGOUTSTREAMINTERFACE_H_

#include <K/Core/OutStreamInterface.h>
#include <K/Core/UniqueHandle.h>

namespace K {
    namespace Core {
        class IoBufferInterface;
    }
}

namespace K {
namespace Core {

//! Interface to nonblocking output streams.
class NonBlockingOutStreamInterface : public virtual OutStreamInterface {
  public:
    //! Writes the specified data to the stream with nonblocking semantics.
    /*!
     *  \return Null-handle in case all data was accepted for writing, or otherwise the data that could not be accepted
     *          at the current time.
     */
    virtual UniqueHandle<IoBufferInterface> WriteNonBlocking(UniqueHandle<IoBufferInterface> buffer) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_NONBLOCKINGOUTSTREAMINTERFACE_H_
