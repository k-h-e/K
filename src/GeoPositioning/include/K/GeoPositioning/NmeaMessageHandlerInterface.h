/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_NMEAMESSAGEHANDLERINTERFACE_H_
#define K_GEOPOSITIONING_NMEAMESSAGEHANDLERINTERFACE_H_

#include <K/Core/DataHandlerInterface.h>

namespace K {
namespace GeoPositioning {

class NmeaMessage;

//! Interface to entities receiving and processing NMEA messages.
class NmeaMessageHandlerInterface : public virtual Core::DataHandlerInterface {
  public:
    //! Asks the handler to process the specified NMEA message.
    /*!
     *  \param id
     *  Activation ID that was registered together with the handler.
     */
    virtual void OnNmeaMessage(int id, const NmeaMessage &message) = 0;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_NMEAMESSAGEHANDLERINTERFACE_H_
