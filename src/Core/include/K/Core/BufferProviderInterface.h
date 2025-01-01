/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BUFFERPROVIDERINTERFACE_H_
#define K_CORE_BUFFERPROVIDERINTERFACE_H_

#include <K/Core/UniqueHandle.h>

namespace K {
    namespace Core {
        class BufferInterface;
    }
}

namespace K {
namespace Core {

//! Interface to entities providing buffers.
class BufferProviderInterface {
  public:
    //! Provides the smallest available buffer capable of holding the specified number of bytes, or the largest
    //! available buffer in case no available buffer is large enough.
    /*!
     *  Always provides a buffer.
     *
     *  The provided buffer remains exlusively allocated to the owner of the passed out unique handle for as long as
     *  that handle - or a potential decendant - lives.
     */ 
    virtual UniqueHandle<BufferInterface> Get(int size) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BUFFERPROVIDERINTERFACE_H_
