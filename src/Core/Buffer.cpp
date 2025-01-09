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

using std::memcpy;
using std::min;
using std::nullopt;
using std::optional;
using std::shared_ptr;

namespace K {
namespace Core {

Buffer::Buffer()
	    : buffer_(256),                                      // Round brackets, want to set size.
		  bufferFill_{0},
          writeCursor_{0},
          error_{false} {}

Buffer::Buffer(int initialSize)
	    : buffer_(initialSize > 0 ? initialSize : 1),        // Round brackets, want to set size.
		  bufferFill_{initialSize > 0 ? initialSize : 0},
          writeCursor_{0},
          error_{false} {}

Buffer::~Buffer() {
    if (closeResultAcceptor_) {
        if (error_) {
            closeResultAcceptor_->OnFailure();
        } else {
            closeResultAcceptor_->OnSuccess();
        }
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

bool Buffer::Empty() const {
    return (bufferFill_ == 0);
}

void Buffer::Clear() {
    bufferFill_  = 0;
    writeCursor_ = 0;
    error_       = false;
}

void Buffer::Shrink(int size) {
    if (!error_) {
        NumberTools::Clamp(size, 0, (int)buffer_.size());
        bufferFill_ = size;
        NumberTools::Clamp(writeCursor_, 0, bufferFill_);
    }
}

void Buffer::Append(const void *data, int dataSize) {
    if (!error_) {
        if (dataSize > 0) {
            int newFill { bufferFill_ + dataSize };
            while (newFill > static_cast<int>(buffer_.size())) {
                Grow();
            }
            if (data) {
                memcpy(&buffer_.front() + bufferFill_, data, dataSize);
            }
            bufferFill_  += dataSize;
            writeCursor_  = bufferFill_;
        }
    }
}

int Buffer::Append(NonBlockingInStreamInterface &stream) {
    if (!error_) {
        auto buffer = stream.ReadNonBlocking();
        if (buffer) {
            Append(buffer->Content(), buffer->Size());
            return buffer->Size();
        }
    }
    
    return 0;
}

void Buffer::RestoreToCurrentCapacity() {
    if (!error_) {
        bufferFill_ = (int)buffer_.size();
    }
}

void Buffer::Grow() {
    if (!error_) {
        buffer_.resize(2u * buffer_.size());
    }
}

bool Buffer::ErrorState() const {
    return error_;
}

optional<StreamInterface::Error> Buffer::StreamError() const {
    if (error_) {
        return Error::User;
     } else {
        return nullopt;
     }
}

void Buffer::SetCloseResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    closeResultAcceptor_ = resultAcceptor;
}

int Buffer::WriteBlocking(const void *data, int dataSize) {
    if (!error_) {
        if (dataSize > 0) {
            assert(writeCursor_ <= bufferFill_);
            int numFree { bufferFill_ - writeCursor_ };                    // >= 0.

            int numFitting { dataSize > numFree ? numFree : dataSize };    // >= 0.
            if (numFitting > 0) {
                memcpy(&buffer_[writeCursor_], data, numFitting);
                writeCursor_ += numFitting;
            }

            int numRemaining { dataSize - numFitting };                    // >= 0.
            if (numRemaining > 0) {
                const uint8_t *remainder { &static_cast<const uint8_t *>(data)[numFitting] };
                Append(remainder, numRemaining);
            }

            return dataSize;
        } else {
            Clear();
            error_ = true;
        }
    }

    return 0;
}

void Buffer::Seek(int64_t position) {
    if (!error_) {
        if ((position < 0) || (position > bufferFill_)) {
            Clear();
            error_ = true;
        }  else {
            writeCursor_ = static_cast<int>(position);
        }
    }
}

void Buffer::RecoverAndSeek(int64_t position) {
    Seek(position);
}

int64_t Buffer::StreamPosition() const {
    return static_cast<int64_t>(writeCursor_);
}

Buffer::Reader Buffer::GetReader() const {
    optional<Error> error { nullopt };
    if (error_) {
        error = Error::User;
    }
    return Reader{this, error};
}

// ---

Buffer::Reader::Reader(const Buffer *buffer, optional<Error> error)
	: buffer_(buffer),
      cursor_(0),
      error_{error} {
    // Nop.
}

bool Buffer::Reader::ErrorState() const {
    return (error_.has_value());
}

optional<StreamInterface::Error> Buffer::Reader::StreamError() const {
    return error_;
}

int Buffer::Reader::ReadBlocking(void *buffer, int bufferSize) {
    assert(bufferSize > 0);
    
    int numRead { 0 };

    if (!ErrorState()) {
        int numToDeliver { buffer_->bufferFill_ - cursor_ };
        if (numToDeliver >= 1) {
            if (numToDeliver > bufferSize) {
                numToDeliver = bufferSize;
            }
            memcpy(buffer, &buffer_->buffer_[cursor_], numToDeliver);
            cursor_ += numToDeliver;
            numRead  = numToDeliver;
        }

        if (!numRead) {
            error_ = Error::Eof;
        }
    }

    return numRead;
}

void Buffer::Reader::Seek(int64_t position) {
    if (!ErrorState()) {
        if ((position >= 0) && (position <= buffer_->bufferFill_)) {
            cursor_ = static_cast<int>(position);
        } else {
            error_ = Error::User;
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
