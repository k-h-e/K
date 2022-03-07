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
#include <K/Core/Result.h>

using namespace std;

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
        if (newFill > (int)buffer_.size()) {
            int newSize = 2 * (int)buffer_.size();
            if (newFill > newSize)
                newSize = newFill;
            buffer_.resize(newSize);
        }
        if (data)
            memcpy(&buffer_.front() + bufferFill_, data, dataSize);
        bufferFill_ += dataSize;
    }
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
