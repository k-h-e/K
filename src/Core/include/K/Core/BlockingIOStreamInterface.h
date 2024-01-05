/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BLOCKINGIOSTREAMINTERFACE_H_
#define K_CORE_BLOCKINGIOSTREAMINTERFACE_H_

#include <K/Core/BlockingInStreamInterface.h>
#include <K/Core/BlockingOutStreamInterface.h>

namespace K {
namespace Core {

//! Interface to blocking input/output streams.
class BlockingIOStreamInterface : public virtual BlockingInStreamInterface,
                                  public virtual BlockingOutStreamInterface {
  public:
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGIOSTREAMINTERFACE_H_
