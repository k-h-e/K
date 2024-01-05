/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_STREAMHANDLERINTERFACE_H_
#define K_CORE_STREAMHANDLERINTERFACE_H_

#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

//! Interface to entities accepting and processing stream data.
class StreamHandlerInterface : public virtual Interface {
  public:
    //! Informs the handler that the stream has entered error state, see <c>StreamInterface::ErrorState()</c>.
    virtual void OnStreamError(StreamInterface::Error error) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_STREAMHANDLERINTERFACE_H_

