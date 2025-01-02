/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/Buffer.h>

#include <cassert>
#include <cstring>
#include <K/Core/IoBufferInterface.h>
#include <K/Core/NonBlockingInStreamInterface.h>
#include <K/Core/NumberTools.h>
#include <K/Core/ResultAcceptor.h>

using std::min;
using std::nullopt;
using std::optional;
using std::shared_ptr;

namespace K {
namespace Core {

Buffer::Buffer()
	    : buffer_(256),
		  bufferFill_(0) {}

Buffer::Buffer(int initialSize)
	    : buffer_(initialSize > 0 ? initialSize : 1),
		  bufferFill_((int)buffer_.size()) {}

Buffer::~Buffer() {
    if (closeResultAcceptor_) {
        closeResultAcceptor_->OnSuccess();
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

void Buffer::AppendFromMemory(const void *data, int dataSize) {
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

int Buffer::AppendFromStream(NonBlockingInStreamInterface *stream) {
    auto buffer = stream->ReadNonBlocking();
    if (buffer) {
        AppendFromMemory(buffer->Content(), buffer->Size());
        return buffer->Size();
    } else {
        return 0;
    }
}

void Buffer::RestoreToCurrentCapacity() {
    bufferFill_ = (int)buffer_.size();
}

void Buffer::Grow() {
    buffer_.resize(2u * buffer_.size());
}

bool Buffer::ErrorState() const {
    return false;
}

optional<StreamInterface::Error> Buffer::StreamError() const {
    return nullopt;
}

void Buffer::SetCloseResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    closeResultAcceptor_ = resultAcceptor;
}

int Buffer::WriteBlocking(const void *data, int dataSize) {
    assert (dataSize > 0);
    AppendFromMemory(data, dataSize);
    return dataSize;
}

Buffer::Reader Buffer::GetReader() const {
    return Reader(this);
}

Buffer::Reader::Reader(const Buffer *buffer)
	: buffer_(buffer),
      cursor_(0) {
}

bool Buffer::Reader::ErrorState() const {
    return (error_.has_value());
}

optional<StreamInterface::Error> Buffer::Reader::StreamError() const {
    return error_;
}

int Buffer::Reader::ReadBlocking(void *buffer, int bufferSize) {
    int numRead = 0;

    if (!ErrorState()) {
        int numToDeliver = buffer_->bufferFill_ - cursor_;
        if (numToDeliver >= 1) {
            if (numToDeliver > bufferSize) {
                numToDeliver = bufferSize;
            }
            memcpy(buffer, &buffer_->buffer_[cursor_], numToDeliver);
            cursor_ += numToDeliver;
            numRead = numToDeliver;
        }

        if (!numRead) {
            error_ = Error::Eof;
        }
    }

    return numRead;
}

void Buffer::Reader::Seek(int64_t position) {
    if (!ErrorState()) {
        if ((position >= 0) && (position < buffer_->bufferFill_)) {
            cursor_ = static_cast<int>(position);
        } else {
            error_ = Error::IO;
        }
    }
}

void Buffer::Reader::RecoverAndSeek(int64_t position) {
    if (error_ == Error::Eof) {
        error_.reset();
        Seek(position);
    }
}

int64_t Buffer::Reader::StreamPosition() const {
    return static_cast<int64_t>(cursor_);
}

}    // Namespace Core.
}    // Namespace K.
