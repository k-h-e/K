/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_NMEAPARSER_H_
#define K_GEOPOSITIONING_NMEAPARSER_H_

#include <memory>
#include <string>
#include <K/Core/RawStreamHandlerInterface.h>
#include <K/GeoPositioning/NmeaMessage.h>

namespace K {
namespace GeoPositioning {

class NmeaMessageHandlerInterface;

//! Parses a binary stream into NMEA messages (sentences).
class NmeaParser : public virtual Core::RawStreamHandlerInterface {
  public:    
    NmeaParser(const std::shared_ptr<NmeaMessageHandlerInterface> &handler);
    NmeaParser()                                   = delete;
    NmeaParser(const NmeaParser &other)            = delete;
    NmeaParser &operator=(const NmeaParser &other) = delete;
    NmeaParser(NmeaParser &&other)                 = delete;
    NmeaParser &operator=(NmeaParser &&other)      = delete;
    ~NmeaParser()                                  = default;

    void OnRawStreamData(const void *data, int dataSize) override;
    void OnStreamError(Core::StreamInterface::Error error) override;

  private:
    enum class State { BetweenMessages,
                       AcceptingType,
                       AcceptingField,
                       AcceptingCheckSum };

    std::shared_ptr<NmeaMessageHandlerInterface> handler_;
    State                                        state_;
    std::string                                  token_;
    NmeaMessage                                  message_;
    int                                          numSkipped_;
    std::string                                  skippedText_;
    bool                                         error_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_NMEAPARSER_H_
