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

#include <K/Core/StreamInterface.h>
#include <K/Core/NonBlockingWriteInterface.h>

namespace K {
namespace Core {

//! Interface to nonblocking output streams.
class NonBlockingOutStreamInterface : public virtual StreamInterface,
                                      public virtual NonBlockingWriteInterface {
  public:
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_NONBLOCKINGOUTSTREAMINTERFACE_H_
