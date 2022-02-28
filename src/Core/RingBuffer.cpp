////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/RingBuffer.h>

#include <cassert>
#include <cstring>

namespace K {
namespace Core {

RingBuffer::RingBuffer()
    : capacity_(256),
      cursor_(0),
      fill_(0) {
    buffer_.resize(capacity_);
}

int RingBuffer::Size() const {
    return fill_;
}

bool RingBuffer::Empty() const {
    return !fill_;
}

int RingBuffer::Get(void *outBuffer, int bufferSize) {
    assert(bufferSize > 0);
    if (fill_) {
        int numToRead = std::min(capacity_ - cursor_, fill_);    // >= 1.
        numToRead     = std::min(numToRead, bufferSize);     // >= 1.

        std::memcpy(outBuffer, &buffer_[cursor_], numToRead);
        cursor_ += numToRead;
        if (cursor_ == capacity_) {
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
    while (capacity_ - fill_ < dataSize) {
        Grow();
    }

    int writePosition = (cursor_ < capacity_ - fill_) ? cursor_ + fill_ : -capacity_ + cursor_ + fill_;
    int numToWrite = std::min(capacity_ - writePosition, dataSize);    // >= 1.
    std::memcpy(&buffer_[writePosition], data, numToWrite);

    int numRemaining = dataSize - numToWrite;
    if (numRemaining) {
        std::memcpy(&buffer_[0], static_cast<const uint8_t *>(data) + numToWrite, numRemaining);
    }

    fill_ += dataSize;
}

void RingBuffer::PutBack(const void *data, int dataSize) {
    assert(dataSize > 0);
    while (capacity_ - fill_ < dataSize) {
        Grow();
    }

    if (dataSize <= cursor_) {
        cursor_ -= dataSize;
        std::memcpy(&buffer_[cursor_], data, dataSize);
    }
    else {
        std::memcpy(&buffer_[0], static_cast<const uint8_t *>(data) - cursor_, cursor_);
        int numRemaining = dataSize - cursor_;    // >= 1.
        cursor_ = capacity_ - numRemaining;
        std::memcpy(&buffer_[cursor_], data, numRemaining);
    }

    fill_ += dataSize;
}

void RingBuffer::TransferTo(RingBuffer *other, int maxSize) {
    while (fill_ && (other->Size() < maxSize)) {
        int numToTransfer = std::min(capacity_ - cursor_, fill_);                // >= 1.
        numToTransfer     = std::min(numToTransfer, maxSize - other->Size());    // >= 1.
        other->Put(&buffer_[cursor_], numToTransfer);
        cursor_ += numToTransfer;
        if (cursor_ == capacity_) {
            cursor_ = 0;
        }
        fill_ -= numToTransfer;
    }
}

void RingBuffer::Grow() {
    int oldCapacity = capacity_;
    capacity_ *= 2;
    buffer_.resize(capacity_);
    if (cursor_) {
        std::memcpy(&buffer_[oldCapacity], &buffer_[0], cursor_);
    }
}

}    // Namespace Core.
}    // Namespace K.
