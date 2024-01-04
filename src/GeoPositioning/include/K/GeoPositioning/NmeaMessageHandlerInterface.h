/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_NMEAMESSAGEHANDLERINTERFACE_H_
#define K_GEOPOSITIONING_NMEAMESSAGEHANDLERINTERFACE_H_

#include <K/Core/StreamHandlerInterface.h>

namespace K {
namespace GeoPositioning {

class NmeaMessage;

//! Interface to entities receiving and processing NMEA messages.
class NmeaMessageHandlerInterface : public virtual Core::StreamHandlerInterface {
  public:
    //! Asks the handler to process the specified NMEA message.
    virtual void OnNmeaMessage(const NmeaMessage &message) = 0;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_NMEAMESSAGEHANDLERINTERFACE_H_
