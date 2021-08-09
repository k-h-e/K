#ifndef K_GEOPOSITIONING_RTCMMESSAGEHANDLERINTERFACE_H_
#define K_GEOPOSITIONING_RTCMMESSAGEHANDLERINTERFACE_H_

#include <K/IO/AsyncDataReceiverInterface.h>

namespace K {
namespace GeoPositioning {

class RtcmMessage;

//! Interface to entities receiving and processing RTCM messages.
class RtcmMessageHandlerInterface : public virtual K::IO::AsyncDataReceiverInterface {
  public:
    virtual void Handle(const RtcmMessage &message) = 0;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_RTCMMESSAGEHANDLERINTERFACE_H_
