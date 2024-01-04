/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/SubStream.h>

#include <cassert>

using std::optional;
using std::shared_ptr;
using K::Core::StreamInterface;

namespace K {
namespace IO {

SubStream::SubStream(const shared_ptr<Core::SeekableBlockingInStreamInterface> &stream, int64_t offset,
                     int64_t size)
        : stream_{stream},
          offset_{offset},
          size_{size},
          position_{0},
          error_{Error::Unspecific} {
    if ((offset_ < 0) || (size_ < 0)) {
        offset_ = 0;
        size_   = 0;
        error_  = Error::User;
    } else {
        stream_->Seek(offset_);
        if (stream_->ErrorState()) {
            error_ = stream_->StreamError();
            assert (error_.has_value());
        } else {
            error_.reset();
        }
    }
}

int SubStream::ReadBlocking(void *buffer, int bufferSize) {
    assert (bufferSize > 0);
    int numRead = 0;
    
    if (!error_) {
        int64_t numLeft = size_ - position_;
        assert (numLeft >= 0);
        int numToRead = numLeft > bufferSize ? bufferSize : static_cast<int>(numLeft);
        if (numToRead > 0) {
            numRead = stream_->ReadBlocking(buffer, numToRead);
            if (!numRead) {
                error_ = stream_->StreamError();
                assert (error_.has_value());
            } else {
                position_ += numRead;
            }
        } else {
            error_ = Error::Eof;
        }
    }
    
    return numRead;
}

void SubStream::Seek(int64_t position) {
    if (!error_) {
        if ((position < 0) || (position >= size_)) {
            error_ = Error::User;
        } else {
            stream_->Seek(offset_ + position_);
            if (stream_->ErrorState()) {
                error_ = stream_->StreamError();
                assert (error_.has_value());
            } else {
                position_ = position;
            }
        }
    }
}

void SubStream::RecoverAndSeek(int64_t position) {
    if (error_ == Error::Eof) {
        if (position < 0) {
            error_ = Error::User;
        } else {
            stream_->RecoverAndSeek(offset_ + position);
            if (stream_->ErrorState()) {
                error_ = stream_->StreamError();
                assert (error_.has_value());
            } else {
                position_ = position;
                error_.reset();
            }
        }
    } else {
        Seek(position);
    }
}

int64_t SubStream::StreamPosition() const {
    return offset_ + position_;
}

bool SubStream::ErrorState() const {
    return (error_.has_value());
}

optional<StreamInterface::Error> SubStream::StreamError() const {
    return error_;
}

}    // Namespace IO.
}    // Namespace K.
