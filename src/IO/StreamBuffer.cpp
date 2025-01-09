/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/StreamBuffer.h>

#include <cassert>
#include <cstring>

#include <K/Core/Log.h>
#include <K/Core/ResultAcceptor.h>
#include <K/IO/IOTools.h>

using std::make_shared;
using std::optional;
using std::shared_ptr;
using std::to_string;
using std::vector;

using K::Core::Log;
using K::Core::ResultAcceptor;
using K::Core::StreamInterface;

namespace K {
namespace IO {

StreamBuffer::StreamBuffer(const shared_ptr<SeekableBlockingIOStreamInterface> &stream, File::AccessMode accessMode,
                           int bufferSize)
        : stream_(stream),
          bufferPosition_(0),
          cursor_(0),
          fill_(0),
          dirty_(false) {
    if (bufferSize < 1) {
        bufferSize = 4096;
    }
    bufferSize_ = bufferSize;
    buffer_     = new uint8_t[bufferSize_];

    File::AccessModeToFlags(accessMode, &readable_, &writable_);

    dirtyBytes_.resize(bufferSize_);

    if (!SetUpBuffer(0)) {
        error_ = Error::IO;
    }
}

StreamBuffer::~StreamBuffer() {
    if (!error_) {
        if (!Flush()) {
            error_ = Error::IO;
        }
    }

    delete[] buffer_;

    if (closeResultAcceptor_) {
        if (error_) {
            closeResultAcceptor_->OnFailure();
        } else {
            closeResultAcceptor_->OnSuccess();
        }
    }

    if (error_ && (error_ != Error::Eof)) {
        Log::Print(Log::Level::Error, this, [&]{ return "failed to properly close!"; });
    }
}

int StreamBuffer::ReadBlocking(void *buffer, int bufferSize) {
    assert(bufferSize > 0);
    int numRead = 0;
    if (!ErrorState()) {
        if (!readable_) {
            error_ = Error::User;
        } else {
            int numToRead = fill_ > cursor_ ? fill_ - cursor_ : 0;
            if (bufferSize < numToRead) {
                numToRead = bufferSize;
            }

            if (numToRead) {
                std::memcpy(buffer, &buffer_[cursor_], numToRead);
                int newCursor = cursor_ + numToRead;
                if (newCursor == bufferSize_) {
                    Seek(bufferPosition_ + bufferSize_);
                    if (!ErrorState()) {
                        numRead = numToRead;
                    } else {
                        Log::Print(Log::Level::Error, this, [&]{ return "error while reading"; });
                    }
                } else {
                    cursor_ = newCursor;
                    numRead = numToRead;
                }
            } else {
                error_ = Error::Eof;
            }
        }
    }

    return numRead;
}

int StreamBuffer::WriteBlocking(const void *data, int dataSize) {
    assert(dataSize > 0);
    int numWritten = 0;  
    if (!ErrorState()) {
        if (!writable_) {
            error_ = Error::User;
        } else {
            int numToWrite = std::min(dataSize, bufferSize_ - cursor_);    // > 0 !
            std::memcpy(&buffer_[cursor_], data, numToWrite);
            int newCursor = cursor_ + numToWrite;
            if (newCursor > fill_) {
                fill_ = newCursor;
            }
            dirty_ = true;

            if (!readable_) {
                for (int i = cursor_; i < newCursor; ++i) {
                    dirtyBytes_[i] = true;
                }
            }

            if (newCursor == bufferSize_) {
                Seek(bufferPosition_ + bufferSize_);
                if (!ErrorState()) {
                    numWritten = numToWrite;
                } else {
                    Log::Print(Log::Level::Error, this, []{ return "error while writing"; });
                }
            } else {
                cursor_    = newCursor;
                numWritten = numToWrite;
            }
        }
    }

    return numWritten;
}

void StreamBuffer::Seek(int64_t position) {
    if (!ErrorState()) {
        if (position >= 0) {
            int64_t newBufferPosition = BufferPositionFor(position);
            if (newBufferPosition != bufferPosition_) {
                if (Flush()) {
                    if (SetUpBuffer(position)) {
                        return;
                    }
                }
            } else {
                cursor_ = static_cast<int>(position - bufferPosition_);
                return;
            }
        }

        if (!ErrorState()) {
            error_ = Error::Unspecific;
        }
        Log::Print(Log::Level::Error, this, [&]{
            return "error while seeking to position " + to_string(position); });
    }
}

void StreamBuffer::RecoverAndSeek(int64_t position) {
    if (error_ == Error::Eof) {
        error_.reset();    // EOF error state was set in ReadBlocking() from valid error-free state.
    }
    Seek(position);
}

int64_t StreamBuffer::StreamPosition() const {
    return bufferPosition_ + static_cast<int64_t>(cursor_);
}

bool StreamBuffer::ErrorState() const {
    return (error_.has_value());
}

optional<StreamInterface::Error> StreamBuffer::StreamError() const {
    return error_;
}

void StreamBuffer::SetCloseResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    closeResultAcceptor_ = resultAcceptor;
    stream_->SetCloseResultAcceptor(resultAcceptor);
}

int64_t StreamBuffer::BufferPositionFor(int64_t position) {
    return (position / static_cast<int64_t>(bufferSize_)) * static_cast<int64_t>(bufferSize_);
}

bool StreamBuffer::SetUpBuffer(int64_t position) {
    int64_t newBufferPosition = BufferPositionFor(position);
    if (readable_) {
        int numReadTotal = 0;
        stream_->RecoverAndSeek(newBufferPosition);
        while (!stream_->ErrorState()) {
            numReadTotal += stream_->ReadBlocking(&buffer_[numReadTotal], bufferSize_ - numReadTotal);
            if ((numReadTotal == bufferSize_) || (stream_->StreamError() == Error::Eof)) {
                if (numReadTotal < bufferSize_) {
                    std::memset(&buffer_[numReadTotal], 0, bufferSize_ - numReadTotal);
                }
                bufferPosition_ = newBufferPosition;
                fill_           = numReadTotal;
                cursor_         = static_cast<int>(position - newBufferPosition);
                dirty_          = false;
                return true;
            }
        }
    } else {
        bufferPosition_ = newBufferPosition;
        fill_           = 0;
        cursor_         = static_cast<int>(position - newBufferPosition);
        dirty_          = false;
        for (vector<bool>::reference bit : dirtyBytes_) {
            bit = false;
        }
        return true;
    }

    Log::Print(Log::Level::Error, this, [&]{
        return "failed to set up buffer for position " + to_string(position);
    });
    return false;
}

bool StreamBuffer::Flush() {
    if (dirty_) {
        // fill_ > 0 !
        if (readable_) {
            if (FlushDirtyRange(0, bufferSize_)) {
                dirty_ = false;
                return true;
            }
        } else {
            bool writeError        = false;
            bool inDirtyRange      = false;
            int  firstInDirtyRange = 0;
            int i = 0;
            while (i < bufferSize_) {
                if (inDirtyRange) {
                    if (!dirtyBytes_[i]) {
                        if (!FlushDirtyRange(firstInDirtyRange, i - firstInDirtyRange)) {
                            writeError = true;
                            break;
                        }
                        inDirtyRange = false;
                    }
                } else {
                    if (dirtyBytes_[i]) {
                        inDirtyRange      = true;
                        firstInDirtyRange = i;
                    }
                }

                ++i;
            }

            if (!writeError && inDirtyRange) {
                if (!FlushDirtyRange(firstInDirtyRange, bufferSize_ - firstInDirtyRange)) {
                    writeError = true;
                }
            }

            if (!writeError) {
                dirty_ = false;
                return true;
            }
        }
    }
    else {
        return true;
    }

    return false;
}

bool StreamBuffer::FlushDirtyRange(int cursor, int numBytes) {
    stream_->RecoverAndSeek(bufferPosition_ + cursor);
    Core::WriteItem(*stream_, &buffer_[cursor], numBytes);
    if (!stream_->ErrorState()) {
        return true;
    } else {
        Log::Print(Log::Level::Error, this, [&]{
            return ("failed to flush " + to_string(numBytes) + " dirty bytes to position "
                + to_string(bufferPosition_ + cursor));
        });
        return false;
    }
}

}    // Namesapce IO.
}    // Namespace K.
