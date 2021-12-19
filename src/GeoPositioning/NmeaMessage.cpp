#include <K/GeoPositioning/NmeaMessage.h>

#include <sstream>
#include <iomanip>
#include <K/Core/ItemReadInterface.h>
#include <K/Core/ItemWriteInterface.h>
#include <K/Core/StringTools.h>

using std::string;
using std::stringstream;
using std::hex;
using std::setw;
using std::setfill;
using std::vector;
using K::Core::ItemReadInterface;
using K::Core::ItemWriteInterface;
using K::Core::StringTools;

namespace K {
namespace GeoPositioning {

NmeaMessage::NmeaMessage(const string &type) {
    Reset(type);
}

void NmeaMessage::Reset(const string &type) {
    type_ = type;
    fields_.clear();
}

bool NmeaMessage::Set(const string &message) {
    vector<string> tokens = StringTools::Tokenize(message, "$", false);
    if (tokens.size() == 2u) {
        tokens = StringTools::Tokenize(tokens[1], "*", false);
        if (tokens.size() == 2u) {
            string checksum = tokens[1];
            tokens = StringTools::Tokenize(tokens[0], ",", false);
            if (tokens.size() > 1u) {
                NmeaMessage newMessage(tokens[0]);
                for (vector<string>::size_type i = 1u; i < tokens.size(); ++i) {
                    newMessage.AddField(tokens[i]);
                }
                if (newMessage.CheckSum() == StringTools::ToLower(checksum)) {
                    *this = newMessage;
                    return true;
                }
            }
        }
    }

    return false;
}

string NmeaMessage::Type() const {
    return type_;
}

bool NmeaMessage::IsOfType(const string &type) const {
    return !type_.compare(type);
}

void NmeaMessage::AddField(const std::string &field) {
    fields_.push_back(field);
}

const string &NmeaMessage::Field(int index) const {
    return fields_[index];
}

string &NmeaMessage::Field(int index) {
    return fields_[index];
}

int NmeaMessage::FieldCount() const {
    return static_cast<int>(fields_.size());
}

string NmeaMessage::CheckSum() const {
    uint8_t checkSum = 0u;
    for (char character : type_ + "," + StringTools::Concatenate(fields_, ",")) {
        checkSum ^= (uint8_t)character;
    }

    stringstream stream;
    stream << hex << setw(2) << setfill('0') << (uint32_t)checkSum;
    return StringTools::ToLower(stream.str());
}

string NmeaMessage::ToString() const {
    return string("$") + type_ + "," + StringTools::Concatenate(fields_, ",") + "*" + CheckSum();
}

void NmeaMessage::WriteTo(ItemWriteInterface *stream) const {
    string text = ToString() + "\r\n";
    stream->WriteItem(text.c_str(), static_cast<int>(text.length()));
}

void NmeaMessage::Serialize(K::Core::ItemWriteInterface *stream) const {
    StringTools::Serialize(type_, stream);
    uint32_t numFields = static_cast<uint32_t>(fields_.size());
    stream->WriteItem(&numFields, sizeof(numFields));
    for (uint32_t i = 0u; i < numFields; ++i) {
        StringTools::Serialize(fields_[i], stream);
    }
}

void NmeaMessage::Deserialize(K::Core::ItemReadInterface *stream) {
    StringTools::Deserialize(&type_, stream);
    uint32_t numFields;
    stream->ReadItem(&numFields, sizeof(numFields));
    if (stream->Good()) {
        fields_.clear();
        for (uint32_t i = 0u; i < numFields; ++i) {
            string field;
            StringTools::Deserialize(&field, stream);
            fields_.push_back(field);
        }
    }
}

}    // Namespace GeoPositioning.
}    // Namespace K.
