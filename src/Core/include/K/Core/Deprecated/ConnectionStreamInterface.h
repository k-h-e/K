/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_DEPRECATED_CONNECTIONSTREAMINTERFACE_H_
#define K_CORE_DEPRECATED_CONNECTIONSTREAMINTERFACE_H_

#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/Deprecated/AsyncInStreamInterface.h>

namespace K {
namespace Core {
namespace Deprecated {

//! Interface to connection streams.
class ConnectionStreamInterface : public virtual AsyncInStreamInterface,
                                  public virtual Core::BlockingOutStreamInterface {
};

}    // Deprecated.
}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_DEPRECATED_CONNECTIONSTREAMINTERFACE_H_
