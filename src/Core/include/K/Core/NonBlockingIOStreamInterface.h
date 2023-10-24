/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_NONBLOCKINGIOSTREAMINTERFACE_H_
#define K_CORE_NONBLOCKINGIOSTREAMINTERFACE_H_

#include <K/Core/NonBlockingInStreamInterface.h>
#include <K/Core/NonBlockingOutStreamInterface.h>

namespace K {
namespace Core {

//! Interface to nonblocking input/output streams.
class NonBlockingIOStreamInterface : public virtual NonBlockingInStreamInterface,
                                     public virtual NonBlockingOutStreamInterface {
  public:
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_NONBLOCKINGIOSTREAMINTERFACE_H_
