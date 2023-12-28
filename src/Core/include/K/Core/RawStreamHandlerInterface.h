/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_RAWSTREAMHANDLERINTERFACE_H_
#define K_CORE_RAWSTREAMHANDLERINTERFACE_H_

#include <K/Core/StreamHandlerInterface.h>

namespace K {
namespace Core {

//! Interface to entities accepting and processing raw stream data.
class RawStreamHandlerInterface : public virtual StreamHandlerInterface {
  public:
    //! Hands over another portion of raw stream data to the handler.
    virtual void OnRawStreamData(const void *data, int dataSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_RAWSTREAMHANDLERINTERFACE_H_

