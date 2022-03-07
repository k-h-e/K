////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_NONBLOCKINGINSTREAMINTERFACE_H_
#define K_CORE_NONBLOCKINGINSTREAMINTERFACE_H_

#include <K/Core/EofInterface.h>
#include <K/Core/NonBlockingReadInterface.h>
#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

//! Interface to nonblocking input streams.
class NonBlockingInStreamInterface : public virtual StreamInterface,
                                     public virtual EofInterface,
                                     public virtual NonBlockingReadInterface {
  public:
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_NONBLOCKINGINSTREAMINTERFACE_H_
