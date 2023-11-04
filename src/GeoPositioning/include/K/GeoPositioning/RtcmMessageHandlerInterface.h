/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_RTCMMESSAGEHANDLERINTERFACE_H_
#define K_GEOPOSITIONING_RTCMMESSAGEHANDLERINTERFACE_H_

#include <K/Core/DataHandlerInterface.h>

namespace K {
namespace GeoPositioning {

class RtcmMessage;

//! Interface to entities receiving and processing RTCM messages.
class RtcmMessageHandlerInterface : public virtual Core::DataHandlerInterface {
  public:
    virtual void OnRtcmMessage(int id, const RtcmMessage &message) = 0;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_RTCMMESSAGEHANDLERINTERFACE_H_
