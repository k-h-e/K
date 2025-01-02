/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_NONBLOCKINGINSTREAMINTERFACE_H_
#define K_CORE_NONBLOCKINGINSTREAMINTERFACE_H_

#include <K/Core/UniqueHandle.h>
#include <K/Core/InStreamInterface.h>

namespace K {
    namespace Core {
        class IoBufferInterface;
    }
}

namespace K {
namespace Core {

//! Interface to nonblocking input streams.
class NonBlockingInStreamInterface : public virtual InStreamInterface {
  public:
    //! Reads data with nonblocking semantics.
    /*!
     *  \return Buffer with read data, or a null-handle in case no data was available for reading.
     */
    virtual UniqueHandle<IoBufferInterface> ReadNonBlocking() = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_NONBLOCKINGINSTREAMINTERFACE_H_
