#ifndef K_GEOPOSITIONING_NMEAMESSAGEHANDLERINTERFACE_H_
#define K_GEOPOSITIONING_NMEAMESSAGEHANDLERINTERFACE_H_

#include <K/IO/DataHandlerInterface.h>

namespace K {
namespace GeoPositioning {

class NmeaMessage;

//! Interface to entities receiving and processing NMEA messages.
class NmeaMessageHandlerInterface : public virtual IO::DataHandlerInterface {
  public:
    virtual void Handle(const NmeaMessage &message) = 0;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_NMEAMESSAGEHANDLERINTERFACE_H_
