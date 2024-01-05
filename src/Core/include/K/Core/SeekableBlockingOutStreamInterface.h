/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SEEKABLEBLOCKINGOUTSTREAMINTERFACE_H_
#define K_CORE_SEEKABLEBLOCKINGOUTSTREAMINTERFACE_H_

#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/SeekableInterface.h>

namespace K {
namespace Core {

//! Interface to seekable blocking output streams.
class SeekableBlockingOutStreamInterface : public virtual BlockingOutStreamInterface,
                                           public virtual SeekableInterface {
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SEEKABLEBLOCKINGOUTSTREAMINTERFACE_H_
