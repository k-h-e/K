#include <K/GeoPositioning/RtcmMessage.h>

#include <K/Core/ItemReadInterface.h>
#include <K/Core/ItemWriteInterface.h>
#include <K/Core/Log.h>

using std::to_string;
using K::Core::ItemReadInterface;
using K::Core::ItemWriteInterface;
using K::Core::Log;

namespace K {
namespace GeoPositioning {

RtcmMessage::RtcmMessage() {
    // Nop.
}

void RtcmMessage::Reset() {
    image_.clear();
}

void RtcmMessage::AppendToImage(const void *data, int dataSize) {
    const uint8_t *ptr = static_cast<const uint8_t *>(data);
    for (int i = 0; i < dataSize; ++i) {
        image_.push_back(*ptr++);
    }
}

int RtcmMessage::ImageSize() const {
    return static_cast<int>(image_.size());
}

bool RtcmMessage::GetType(int *outType) const {
    if (image_.size() >= 5u) {
        uint32_t hi = static_cast<uint32_t>(image_[3]);
        uint32_t lo = static_cast<uint32_t>(image_[4]);
        *outType = static_cast<int>((hi << 4) | (lo >> 4));
        return true;
    }
    return false;
}

int RtcmMessage::PayloadSize() const {
    if (image_.size() >= 3u) {
        uint32_t hi = static_cast<uint32_t>(image_[1]);
        uint32_t lo = static_cast<uint32_t>(image_[2]);
        return static_cast<int>(((hi << 8) | lo) & 0x3ffu);
    }

    return 0;
}

void RtcmMessage::WriteTo(ItemWriteInterface *stream) const {
    if (image_.size()) {
        stream->WriteItem(&image_[0], static_cast<int>(image_.size()));
    }
}

void RtcmMessage::Serialize(ItemWriteInterface *stream) const {
    uint32_t size = static_cast<uint32_t>(image_.size());
    stream->WriteItem(&size, sizeof(size));
    if (size) {
        stream->WriteItem(&image_[0], static_cast<int>(size));
    }
}

void RtcmMessage::Deserialize(ItemReadInterface *stream) {
    uint32_t size;
    stream->ReadItem(&size, sizeof(size));
    if (stream->Good() && size) {
        image_.resize(size);
        stream->ReadItem(&image_[0], static_cast<int>(size));
    }
}

}    // Namespace GeoPositioning.
}    // Namespace K.
