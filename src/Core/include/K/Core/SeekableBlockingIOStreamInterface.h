/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SEEKABLEBLOCKINGIOSTREAMINTERFACE_H_
#define K_CORE_SEEKABLEBLOCKINGIOSTREAMINTERFACE_H_

#include <K/Core/SeekableBlockingInStreamInterface.h>
#include <K/Core/SeekableBlockingOutStreamInterface.h>

namespace K {
namespace Core {

//! Interface to seekable blocking input/output streams.
class SeekableBlockingIOStreamInterface : public virtual SeekableBlockingInStreamInterface,
                                          public virtual SeekableBlockingOutStreamInterface {
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SEEKABLEBLOCKINGIOSTREAMINTERFACE_H_
