#include <K/GeoPositioning/NmeaMessage.h>

#include <sstream>
#include <iomanip>
#include <K/Core/ItemWriteInterface.h>
#include <K/Core/StringTools.h>

using std::string;
using std::stringstream;
using std::hex;
using std::setw;
using std::setfill;
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

}    // Namespace GeoPositioning.
}    // Namespace K.
