////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_STREAMINTERFACE_H_
#define K_CORE_STREAMINTERFACE_H_

#include <K/Core/ErrorStateInterface.h>
#include <K/Core/FinalResultInterface.h>

namespace K {
namespace Core {

//! Interface to entities providing stream I/O.
class StreamInterface : public virtual ErrorStateInterface,
                        public virtual FinalResultInterface {
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_STREAMINTERFACE_H_
