#include <K/Core/RingBuffer.h>

#include <cassert>
#include <cstring>

namespace K {
namespace Core {

RingBuffer::RingBuffer()
    : size_(256),
      cursor_(0),
      fill_(0) {
    buffer_.resize(size_);
}

int RingBuffer::Fill() {
    return fill_;
}

int RingBuffer::Get(void *outBuffer, int bufferSize) {
    assert(bufferSize > 0);
    if (fill_) {
        int numToRead = std::min(size_ - cursor_, fill_);    // >= 1.
        numToRead     = std::min(numToRead, bufferSize);     // >= 1.

        std::memcpy(outBuffer, &buffer_[cursor_], numToRead);
        cursor_ += numToRead;
        if (cursor_ == size_) {
            cursor_ = 0;
        }
        fill_ -= numToRead;

        return numToRead;
    }
    else {
        return 0;
    }
}

void RingBuffer::Put(const void *data, int dataSize) {
    assert(dataSize > 0);
    while (size_ - fill_ < dataSize) {
        Grow();
    }

    int writePosition = (cursor_ < size_ - fill_) ? cursor_ + fill_ : -size_ + cursor_ + fill_;
    int numToWrite = std::min(size_ - writePosition, dataSize);    // >= 1.
    std::memcpy(&buffer_[writePosition], data, numToWrite);

    int numRemaining = dataSize - numToWrite;
    if (numRemaining) {
        std::memcpy(&buffer_[0], static_cast<const uint8_t *>(data) + numToWrite, numRemaining);
    }

    fill_ += dataSize;
}

void RingBuffer::Grow() {
    int oldSize = size_;
    size_ *= 2;
    buffer_.resize(size_);
    if (cursor_) {
        std::memcpy(&buffer_[oldSize], &buffer_[0], cursor_);
    }
}

}    // Namespace Core.
}    // Namespace K.
