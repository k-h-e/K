/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_RTCMMESSAGEHANDLERINTERFACE_H_
#define K_GEOPOSITIONING_RTCMMESSAGEHANDLERINTERFACE_H_

#include <K/Core/StreamHandlerInterface.h>

namespace K {
namespace GeoPositioning {

class RtcmMessage;

//! Interface to entities receiving and processing RTCM messages.
class RtcmMessageHandlerInterface : public virtual Core::StreamHandlerInterface {
  public:
    virtual void OnRtcmMessage(const RtcmMessage &message) = 0;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_RTCMMESSAGEHANDLERINTERFACE_H_
