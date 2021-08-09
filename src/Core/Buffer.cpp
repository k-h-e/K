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
    finalResultAcceptor_.reset();
    return *this;
}

Buffer::~Buffer() {
    if (finalResultAcceptor_) {
        finalResultAcceptor_->Set(true);
    }
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

bool Buffer::Good() const {
    return true;
}

bool Buffer::ErrorState() {
    return false;
}

void Buffer::SetFinalResultAcceptor(const shared_ptr<Result> &resultAcceptor) {
    finalResultAcceptor_ = resultAcceptor;
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
      eof_(false) {
}

Buffer::Reader::~Reader() {
    if (finalResultAcceptor_) {
        finalResultAcceptor_->Set(true);
    }
}

int Buffer::Reader::Read(void *targetBuffer, int targetBufferSize) {
    int numToDeliver = buffer_->bufferFill_ - cursor_;
    if (eof_ || (numToDeliver < 1)) {
        eof_ = true;
        return 0;
    }
    if (numToDeliver > targetBufferSize) {
        numToDeliver = targetBufferSize;
    }
    memcpy(targetBuffer, &buffer_->buffer_[cursor_], numToDeliver);
    cursor_ += numToDeliver;
    return numToDeliver;
}
    
bool Buffer::Reader::ReadBlock(void *targetBuffer, int targetBufferSize) {
    return (Read(targetBuffer, targetBufferSize) == targetBufferSize);
}

void Buffer::Reader::ReadItem(void *outItem, int itemSize) {
    (void)ReadBlock(outItem, itemSize);
}

bool Buffer::Reader::Good() const {
    return !eof_;
}

bool Buffer::Reader::ErrorState() {
    return false;
}

bool Buffer::Reader::Eof() {
    return eof_;
}

void Buffer::Reader::ClearEof() {
    eof_ = false;
}

void Buffer::Reader::SetFinalResultAcceptor(const shared_ptr<Result> &resultAcceptor) {
    finalResultAcceptor_ = resultAcceptor;
}

}    // Namespace Core.
}    // Namespace K.
