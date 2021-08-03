#include <K/GeoPositioning/NmeaParser.h>

#include <K/Core/Log.h>
#include <K/Core/StringTools.h>
#include <K/GeoPositioning/NmeaMessageHandlerInterface.h>

using std::shared_ptr;
using std::string;
using std::vector;
using K::Core::Log;
using K::Core::StringTools;

namespace K {
namespace GeoPositioning {

NmeaParser::NmeaParser(const shared_ptr<NmeaMessageHandlerInterface> &handler)
        : handler_(handler),
          state_(State::BetweenMessages),
          message_("") {
    // Nop.
}

void NmeaParser::OnDataRead(const void *data, int dataSize) {
    const uint8_t *dataPtr = static_cast<const uint8_t *>(data);
    for (int i = 0; i < dataSize; ++i) {
        char character = static_cast<char>(*dataPtr++);

        if (character == '$') {
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
                    Log::Print(Log::Level::Debug, this, [&]{
                        return "received NMEA message: \"" + message_.ToString() + "\"";
                    });
                    handler_->Handle(message_);
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
                break;
        }
    }
}

void NmeaParser::OnEof() {
    // Nop.
}

void NmeaParser::OnError() {
    // Nop.
}

}    // Namespace GeoPositioning.
}    // Namespace K.

