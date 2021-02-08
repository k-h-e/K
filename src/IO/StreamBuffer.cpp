#include <K/IO/StreamBuffer.h>

#include <cassert>
#include <K/Core/Result.h>
#include <K/Core/Log.h>

using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::to_string;
using K::Core::Result;
using K::Core::Log;

namespace K {
namespace IO {

StreamBuffer::StreamBuffer(const shared_ptr<SeekableBlockingStreamInterface> &stream, File::AccessMode accessMode,
                           int bufferSize)
        : StreamBuffer(stream, accessMode, bufferSize, nullptr) {
    // Nop.
}

StreamBuffer::StreamBuffer(const shared_ptr<SeekableBlockingStreamInterface> &stream, File::AccessMode accessMode,
                           int bufferSize, const shared_ptr<Result> &resultAcceptor)
        : stream_(stream),
          bufferPosition_(0),
          cursor_(0),
          fill_(0),
          dirty_(false),
          eof_(false),
          error_(false),
          finalResultAcceptor_(resultAcceptor) {
    if (bufferSize < 1) {
        bufferSize = 4096;
    }
    bufferSize_ = bufferSize;
    buffer_     = new uint8_t[bufferSize_];

    File::AccessModeToFlags(accessMode, &readable_, &writable_);

    dirtyBytes_.resize(bufferSize_);

    if (!SetUpBuffer(0)) {
        error_ = true;
    }
}

StreamBuffer::~StreamBuffer() {
    if (!error_) {
        if (!Flush()) {
            error_ = true;
        }
    }

    auto streamResult = make_shared<Result>();
    stream_->SetFinalResultAcceptor(streamResult);
    stream_.reset();

    delete[] buffer_;

    if (finalResultAcceptor_) {
        if (error_) {
            finalResultAcceptor_->Set(false);
        }
        else {
            if (streamResult->Success()) {
                finalResultAcceptor_->Set(true);
            }
            else if (streamResult->Failure()) {
                finalResultAcceptor_->Set(false);
            }
        }
    }
}

int StreamBuffer::Read(void *outBuffer, int bufferSize) {
    assert(bufferSize > 0);
    if (!error_ && !eof_) {
        if (readable_) {
            int numToRead = fill_ > cursor_ ? fill_ - cursor_ : 0;
            if (bufferSize < numToRead) {
                numToRead = bufferSize;
            }

            if (numToRead) {
                std::memcpy(outBuffer, &buffer_[cursor_], numToRead);
                int newCursor = cursor_ + numToRead;
                if (newCursor == bufferSize_) {
                    if (Seek(bufferPosition_ + bufferSize_)) {
                        return numToRead;
                    }
                }
                else {
                    cursor_ = newCursor;
                    return numToRead;
                }
            }
            else {
                eof_ = true;
                return 0;
            }
        }

        error_ = true;
        Log::Print(Log::Level::Warning, this, []{ return "error while reading"; });
    }

    return 0;
}

int StreamBuffer::Write(const void *data, int dataSize) {
    assert(dataSize > 0);
    if (!error_) {
        if (writable_) {
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
                if (Seek(bufferPosition_ + bufferSize_)) {
                    return numToWrite;
                }
            }
            else {
                cursor_ = newCursor;
                return numToWrite;
            }
        }

        error_ = true;
        Log::Print(Log::Level::Warning, this, []{ return "error while writing"; });
    }

    return 0;
}

bool StreamBuffer::Seek(int64_t position) {
    assert(position >= 0);
    if (!error_) {
        int newBufferPosition = BufferPositionFor(position);
        if (newBufferPosition != bufferPosition_) {
            if (Flush()) {
                if (SetUpBuffer(position)) {
                    return true;
                }
            }
        }
        else {
            cursor_ = position - bufferPosition_;
            return true;
        }

        error_ = true;
        Log::Print(Log::Level::Warning, this, [&]{
            return "error while seeking to position " + to_string(position); });
    }

    return false;
}

int64_t StreamBuffer::StreamPosition() {
    return bufferPosition_ + static_cast<int64_t>(cursor_);
}

bool StreamBuffer::Eof() {
    return eof_;
}

void StreamBuffer::ClearEof() {
    eof_ = false;
}

bool StreamBuffer::ErrorState() {
    return error_;
}

void StreamBuffer::SetFinalResultAcceptor(const shared_ptr<Result> &resultAcceptor) {
    finalResultAcceptor_ = resultAcceptor;
}

int64_t StreamBuffer::BufferPositionFor(int64_t position) {
    return (position / static_cast<int64_t>(bufferSize_)) * static_cast<int64_t>(bufferSize_);
}

bool StreamBuffer::SetUpBuffer(int64_t position) {
    int64_t newBufferPosition = BufferPositionFor(position);
    if (readable_) {
        stream_->ClearEof();
        if (stream_->Seek(newBufferPosition)) {
            int numReadTotal = 0;
            while (!stream_->ErrorState()) {
                numReadTotal += stream_->Read(&buffer_[numReadTotal], bufferSize_ - numReadTotal);
                if ((numReadTotal == bufferSize_) || stream_->Eof()) {
                    if (numReadTotal < bufferSize_) {
                        std::memset(&buffer_[numReadTotal], 0, bufferSize_ - numReadTotal);
                    }
                    bufferPosition_ = newBufferPosition;
                    fill_           = numReadTotal;
                    cursor_         = position - newBufferPosition;
                    dirty_          = false;
                    Log::Print(Log::Level::Debug, this, [&]{
                        return "pre-loaded buffer with " + to_string(fill_) + " bytes from position "
                            + to_string(bufferPosition_);
                    });
                    return true;
                }
            }
        }
    }
    else {
        bufferPosition_ = newBufferPosition;
        fill_           = 0;
        cursor_         = position - newBufferPosition;
        dirty_          = false;
        for (vector<bool>::reference bit : dirtyBytes_) {
            bit = false;
        }
        Log::Print(Log::Level::Debug, this, [&]{
            return "set up buffer for write-only access to position " + to_string(bufferPosition_);
        });
        return true;
    }

    Log::Print(Log::Level::Warning, this, [&]{
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
        }
        else {
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
                }
                else {
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
    if (stream_->Seek(bufferPosition_ + cursor)) {
        if (stream_->WriteItem(&buffer_[cursor], numBytes)) {
            Log::Print(Log::Level::Debug, this, [&]{
                return "flushed " + to_string(numBytes) + " dirty bytes to position "
                    + to_string(bufferPosition_ + cursor);
            });
            return true;
        }
    }

    Log::Print(Log::Level::Warning, this, [&]{
        return ("failed to flush " + to_string(numBytes) + " dirty bytes to position "
            + to_string(bufferPosition_ + cursor));
    });
    return false;
}

}    // Namesapce IO.
}    // Namespace K.
