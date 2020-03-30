#include <K/Core/Buffer.h>

#include <cassert>
#include <cstring>
#include <K/Core/NumberTools.h>

using namespace std;

namespace K {
namespace Core {

Buffer::Buffer()
	    : buffer_(256),
		  bufferFill_(0) {}

Buffer::Buffer(int initialSize)
	    : buffer_(initialSize > 0 ? initialSize : 1),
		  bufferFill_((int)buffer_.size()) {}

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

void Buffer::RestoreToCurrentCapacity() {
    bufferFill_ = (int)buffer_.size();
}

Buffer::Reader Buffer::GetReader() const {
    return Reader(this);
}

Buffer::Reader::Reader(const Buffer *buffer)
	: buffer_(buffer),
	  cursor_(0) {
}

int Buffer::Reader::Read(void *targetBuffer, int targetBufferSize) {
    int numToDeliver = buffer_->bufferFill_ - cursor_;
    if (numToDeliver < 1)
        return 0;
    if (numToDeliver > targetBufferSize)
        numToDeliver = targetBufferSize;
    memcpy(targetBuffer, &buffer_->buffer_[cursor_], numToDeliver);
    cursor_ += numToDeliver;
    return numToDeliver;
}
    
}    // Namespace Core.
}    // Namespace K.
