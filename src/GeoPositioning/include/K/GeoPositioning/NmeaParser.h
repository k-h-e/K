/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_NMEAPARSER_H_
#define K_GEOPOSITIONING_NMEAPARSER_H_

#include <memory>
#include <string>
#include <K/Core/StreamHandlerInterface.h>
#include <K/GeoPositioning/NmeaMessage.h>

namespace K {
namespace GeoPositioning {

class NmeaMessageHandlerInterface;

//! Parses a binary stream into NMEA messages (sentences).
class NmeaParser : public virtual Core::StreamHandlerInterface {
  public:    
    NmeaParser(const std::shared_ptr<NmeaMessageHandlerInterface> &handler, int activationId);
    NmeaParser()                                   = delete;
    NmeaParser(const NmeaParser &other)            = delete;
    NmeaParser &operator=(const NmeaParser &other) = delete;
    NmeaParser(NmeaParser &&other)                 = delete;
    NmeaParser &operator=(NmeaParser &&other)      = delete;
    ~NmeaParser()                                  = default;

    void OnStreamData(int id, const void *data, int dataSize) override;
    void OnStreamEnteredErrorState(int id, Core::StreamInterface::Error error) override;

  private:
    enum class State { BetweenMessages,
                       AcceptingType,
                       AcceptingField,
                       AcceptingCheckSum };

    std::shared_ptr<NmeaMessageHandlerInterface> handler_;
    int                                          handlerActivationId_;
    State                                        state_;
    std::string                                  token_;
    NmeaMessage                                  message_;
    int                                          numSkipped_;
    std::string                                  skippedText_;
    bool                                         eof_;
    bool                                         error_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_NMEAPARSER_H_
