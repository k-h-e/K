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

#include <K/Core/BlockingWriteInterface.h>
#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

//! Interface to blocking output streams.
class BlockingOutStreamInterface : public virtual StreamInterface,
                                   public virtual BlockingWriteInterface {
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGOUTSTREAMINTERFACE_H_
