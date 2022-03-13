////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BLOCKINGINSTREAMINTERFACE_H_
#define K_CORE_BLOCKINGINSTREAMINTERFACE_H_

#include <K/Core/BlockingReadInterface.h>
#include <K/Core/EofInterface.h>
#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

//! Interface to blocking input streams.
class BlockingInStreamInterface : public virtual StreamInterface,
                                  public virtual EofInterface,
                                  public virtual BlockingReadInterface {
  public:
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGINSTREAMINTERFACE_H_
