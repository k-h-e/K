/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_RTCMPARSER_H_
#define K_GEOPOSITIONING_RTCMPARSER_H_

#include <memory>
#include <vector>
#include <K/Core/StreamHandlerInterface.h>
#include <K/GeoPositioning/RtcmMessage.h>

namespace K {
namespace GeoPositioning {

class RtcmMessageHandlerInterface;

//! Parses a binary stream into RTCM messages.
class RtcmParser : public virtual Core::StreamHandlerInterface {
  public:
    RtcmParser(const std::shared_ptr<RtcmMessageHandlerInterface> &handler, int handlerActivationId);
    RtcmParser(const RtcmParser &other)            = delete;
    RtcmParser &operator=(const RtcmParser &other) = delete;
    RtcmParser(RtcmParser &&other)                 = delete;
    RtcmParser &operator=(RtcmParser &&other)      = delete;

    void OnStreamData(int id, const void *data, int dataSize) override;
    void OnStreamEnteredErrorState(int id, Core::StreamInterface::Error error) override;

  private:
    enum class State { BetweenMessages,
                       AcceptingHeader,
                       AcceptingPayload,
                       AcceptingCrc      };

    std::shared_ptr<RtcmMessageHandlerInterface> handler_;
    int                                          handlerActivationId_;
    State                                        state_;
    int                                          payloadSize_;
    int                                          numSkipped_;
    RtcmMessage                                  message_;
    bool                                         eof_;
    bool                                         error_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_RTCMPARSER_H_
