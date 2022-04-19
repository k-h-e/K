////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/Buffer.h>

#include <cassert>
#include <cstring>
#include <K/Core/NumberTools.h>

using std::min;

namespace K {
namespace Core {

Buffer::Buffer()
	    : buffer_(256),
		  bufferFill_(0) {}

Buffer::Buffer(int initialSize)
	    : buffer_(initialSize > 0 ? initialSize : 1),
		  bufferFill_((int)buffer_.size()) {}

Buffer::Buffer(const Buffer &other) {
    *this = other;
}

Buffer &Buffer::operator=(const Buffer &other) {
    buffer_     = other.buffer_;
    bufferFill_ = other.bufferFill_;
    return *this;
}

void *Buffer::Data() {
    return &buffer_.front();
}

const void *Buffer::Data() const {
    return &buffer_.front();
}

int Buffer::DataSize() const {
    return bufferFill_;
}

void Buffer::Clear() {
    bufferFill_ = 0;
}

void Buffer::Shrink(int size) {
    NumberTools::Clamp(&size, 0, (int)buffer_.size());
    bufferFill_ = size;
}

void Buffer::Append(const void *data, int dataSize) {
    if (dataSize > 0) {
        int newFill = bufferFill_ + dataSize;
        while (newFill > static_cast<int>(buffer_.size())) {
           Grow();
        }
        if (data) {
            memcpy(&buffer_.front() + bufferFill_, data, dataSize);
        }
        bufferFill_ += dataSize;
    }
}

int Buffer::AppendFromReader(NonBlockingReadInterface *reader, int maxNumBytes) {
    assert(maxNumBytes > 0);
    int numFree = static_cast<int>(buffer_.size()) - bufferFill_;
    if (!numFree) {
        Grow();
        numFree = static_cast<int>(buffer_.size()) - bufferFill_;
    }
    int numRead = reader->ReadNonBlocking(&buffer_[bufferFill_], min(numFree, maxNumBytes));
    bufferFill_ += numRead;
    return numRead;
}

void Buffer::WriteItem(const void *item, int itemSize) {
    Append(item, itemSize);
}

bool Buffer::WriteFailed() const {
    return false;
}

void Buffer::ClearWriteFailed() {
    // Nop.
}

void Buffer::RestoreToCurrentCapacity() {
    bufferFill_ = (int)buffer_.size();
}

void Buffer::Grow() {
    buffer_.resize(2u * buffer_.size());
}

Buffer::Reader Buffer::GetReader() const {
    return Reader(this);
}

Buffer::Reader::Reader(const Buffer *buffer)
	: buffer_(buffer),
      cursor_(0),
      readFailed_(false) {
}

int Buffer::Reader::ReadNonBlocking(void *buffer, int bufferSize) {
    int numRead = 0;
    if (!readFailed_) {
        int numToDeliver = buffer_->bufferFill_ - cursor_;
        if (numToDeliver >= 1) {
            if (numToDeliver > bufferSize) {
                numToDeliver = bufferSize;
            }
            memcpy(buffer, &buffer_->buffer_[cursor_], numToDeliver);
            cursor_ += numToDeliver;
            numRead = numToDeliver;
        }
    }

    return numRead;
}

void Buffer::Reader::ReadItem(void *item, int itemSize) {
    if (!readFailed_) {
       if (ReadNonBlocking(item, itemSize) != itemSize) {
           readFailed_ = true;
       }
    }
}

bool Buffer::Reader::ReadFailed() const {
    return readFailed_;
}

void Buffer::Reader::ClearReadFailed() {
    readFailed_ = false;
}

}    // Namespace Core.
}    // Namespace K.
