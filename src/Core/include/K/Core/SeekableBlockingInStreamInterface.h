/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SEEKABLEBLOCKINGINSTREAMINTERFACE_H_
#define K_CORE_SEEKABLEBLOCKINGINSTREAMINTERFACE_H_

#include <K/Core/BlockingInStreamInterface.h>
#include <K/Core/SeekableInterface.h>

namespace K {
namespace Core {

//! Interface to seekable blocking input streams.
class SeekableBlockingInStreamInterface : public virtual BlockingInStreamInterface,
                                          public virtual SeekableInterface {
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SEEKABLEBLOCKINGINSTREAMINTERFACE_H_
