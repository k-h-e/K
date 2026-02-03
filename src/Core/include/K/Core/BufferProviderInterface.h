/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BUFFERPROVIDERINTERFACE_H_
#define K_CORE_BUFFERPROVIDERINTERFACE_H_

#include <K/Core/Interface.h>
#include <K/Core/UniqueHandle.h>

namespace K {
    namespace Core {
        class ByteSpanInterface;
    }
}

namespace K {
namespace Core {

//! Interface to entities capable of providing buffers.
class BufferProviderInterface : public virtual Interface {
  public:
    //! Provides an I/O buffer of the size specified, or a smaller one in case the requested size exceeds the maximum
    //! supported buffer size.
    /*!
     *  The provided buffer remains exlusively allocated to the owner of the passed out unique handle for as long as
     *  that handle - or a potential decendant - lives.
     * 
     *  \return buffer.
     */ 
    virtual UniqueHandle<ByteSpanInterface> Get(int size) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BUFFERPROVIDERINTERFACE_H_
