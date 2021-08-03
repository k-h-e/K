#ifndef K_GEOPOSITIONING_RTCMPARSER_H_
#define K_GEOPOSITIONING_RTCMPARSER_H_

#include <memory>
#include <vector>
#include <K/IO/AsyncReadInterface.h>
#include <K/GeoPositioning/RtcmMessage.h>

namespace K {
namespace GeoPositioning {

class RtcmMessageHandlerInterface;

//! Parses a binary stream into RTCM messages.
class RtcmParser : public virtual K::IO::AsyncReadInterface::HandlerInterface {
  public:
    RtcmParser(const std::shared_ptr<RtcmMessageHandlerInterface> &handler);
    RtcmParser(const RtcmParser &other)            = delete;
    RtcmParser &operator=(const RtcmParser &other) = delete;
    RtcmParser(RtcmParser &&other)                 = delete;
    RtcmParser &operator=(RtcmParser &&other)      = delete;

    void OnDataRead(const void *data, int dataSize) override;
    void OnEof() override;
    void OnError() override;

  private:
    enum class State { BetweenMessages,
                       AcceptingHeader,
                       AcceptingPayload,
                       AcceptingCrc      };

    std::shared_ptr<RtcmMessageHandlerInterface> handler_;
    State                                        state_;
    int                                          payloadSize_;
    int                                          numSkipped_;
    RtcmMessage                                  message_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_RTCMPARSER_H_
