/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/GeoPositioning/NmeaParser.h>

#include <cassert>
#include <K/Core/Log.h>
#include <K/Core/StringTools.h>
#include <K/GeoPositioning/NmeaMessageHandlerInterface.h>

using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;
using K::Core::Log;
using K::Core::StreamInterface;
using K::Core::StringTools;

namespace K {
namespace GeoPositioning {

NmeaParser::NmeaParser(const shared_ptr<NmeaMessageHandlerInterface> &handler)
        : handler_(handler),
          state_(State::BetweenMessages),
          message_(""),
          numSkipped_(0),
          error_(false) {
    // Nop.
}

void NmeaParser::OnRawStreamData(const void *data, int dataSize) {
    if (!error_) {
        const uint8_t *dataPtr = static_cast<const uint8_t *>(data);
        for (int i = 0; i < dataSize; ++i) {
            char character = static_cast<char>(*dataPtr++);

            if (character == '$') {
                if (numSkipped_) {
                    Log::Print(Log::Level::Warning, this, [&]{
                        return "skipped " + to_string(numSkipped_) + " bytes between messages: \"" + skippedText_
                            + "\"";
                    });
                    numSkipped_ = 0;
                    skippedText_.clear();
                }
                token_.clear();
                state_ = State::AcceptingType;
            } else switch (state_) {
                case State::AcceptingType:
                    if (character == ',') {
                        message_.Reset(token_);
                        token_.clear();
                        state_ = State::AcceptingField;
                    } else if (token_.length() >= 80u) {
                        Log::Print(Log::Level::Warning, this, []{ return "type too long"; });
                        state_ = State::BetweenMessages;
                    } else {
                        token_.push_back(character);
                    }
                    break;
                case State::AcceptingField:
                    if (character == '*') {
                        message_.AddField(token_);
                        token_.clear();
                        state_ = State::AcceptingCheckSum;
                    } else if (character == ',') {
                        message_.AddField(token_);
                        token_.clear();
                        state_ = State::AcceptingField;
                    } else if (token_.length() >= 80u) {
                        Log::Print(Log::Level::Warning, this, []{ return "field too long"; });
                        state_ = State::BetweenMessages;
                    } else {
                        token_.push_back(character);
                    }
                    break;
            case State::AcceptingCheckSum:
                if (character == '\n') {
                    if (message_.CheckSum() == StringTools::ToLower(token_)) {
                        handler_->OnNmeaMessage(message_);
                    } else {
                        Log::Print(Log::Level::Warning, this, [&]{
                            return "checksum mismatch: \"" + message_.ToString() + "\", expected_checksum=\"" + token_
                                + "\"";
                        });
                    }
                    token_.clear();
                    state_ = State::BetweenMessages;
                } else if (character == '\r') {
                    // Nop.
                } else if (token_.length() >= 2u) {
                    Log::Print(Log::Level::Warning, this, []{ return "checksum too long"; });
                    state_ = State::BetweenMessages;
                } else {
                    token_.push_back(character);
                }
                break;
            case State::BetweenMessages:
                default:
                    ++numSkipped_;
                    skippedText_.push_back(character);
                    break;
            }
        }
    }
}

void NmeaParser::OnStreamError(StreamInterface::Error error) {
    if (!error_) {
        handler_->OnStreamError(error);
        error_ = true;
    }
}

}    // Namespace GeoPositioning.
}    // Namespace K.

