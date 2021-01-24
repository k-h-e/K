#include <K/IO/StreamBuffer.h>

#include <cassert>
#include <K/Core/Log.h>

using std::shared_ptr;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

StreamBuffer::StreamBuffer(const shared_ptr<SeekableBlockingStreamInterface> &stream, int bufferSize)
        : stream_(stream),
          bufferPosition_(0),
          cursor_(0),
          fill_(0),
          dirty_(false),
          error_(false) {
    if (bufferSize < 1) {
        bufferSize = 4096;
    }
    bufferSize_ = bufferSize;
    buffer_     = new uint8_t[bufferSize_];

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

    delete[] buffer_;
}

int StreamBuffer::Read(void *outBuffer, int bufferSize) {
    assert(bufferSize > 0);

    int numToRead = fill_ > cursor_ ? fill_ - cursor_ : 0;
    if (bufferSize < numToRead) {
        numToRead = bufferSize;
    }

    if (numToRead) {
        if (!error_) {
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

            error_ = true;
            Log::Print(Log::Level::Warning, this, []{ return "read error"; });
        }
    }

    return 0;
}

int StreamBuffer::Write(const void *data, int dataSize) {
    assert(dataSize > 0);
    if (!error_) {
        int numToWrite = std::min(dataSize, bufferSize_ - cursor_);    // > 0 !
        std::memcpy(&buffer_[cursor_], data, numToWrite);
        int newCursor = cursor_ + numToWrite;
        if (newCursor > fill_) {
            fill_ = newCursor;
        }
        dirty_ = true;

        if (newCursor == bufferSize_) {
            if (Seek(bufferPosition_ + bufferSize_)) {
                return numToWrite;
            }
        }
        else {
            cursor_ = newCursor;
            return numToWrite;
        }

        error_ = true;
        Log::Print(Log::Level::Warning, this, []{ return "write error"; });
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
            return "error encountered while seeking to position " + to_string(position); });
    }

    return false;
}

int64_t StreamBuffer::StreamPosition() {
    return bufferPosition_ + static_cast<int64_t>(cursor_);
}

bool StreamBuffer::Eof() {
    return (cursor_ >= fill_);
}

bool StreamBuffer::ErrorState() {
    return error_;
}

int64_t StreamBuffer::BufferPositionFor(int64_t position) {
    return (position / static_cast<int64_t>(bufferSize_)) * static_cast<int64_t>(bufferSize_);
}

bool StreamBuffer::Flush() {
    if (dirty_) {
        // fill_ > 0 !
        if (stream_->Seek(bufferPosition_)) {
            if (stream_->WriteItem(buffer_, fill_)) {
                dirty_ = false;
                Log::Print(Log::Level::Debug, this, [&]{
                    return "flushed " + to_string(fill_) + " dirty bytes to position " + to_string(bufferPosition_);
                });
                return true;
            }
        }
    }
    else {
        return true;
    }

    Log::Print(Log::Level::Warning, this, [&]{
        return ("failed to flush " + to_string(fill_) + " dirty bytes to position " + to_string(bufferPosition_));
    });
    return false;
}

bool StreamBuffer::SetUpBuffer(int64_t position) {
    int64_t newBufferPosition = BufferPositionFor(position);
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
                    return "pre-loaded " + to_string(fill_) + " bytes from position " + to_string(bufferPosition_);
                });
                return true;
            }
        }
    }

    Log::Print(Log::Level::Warning, this, [&]{
        return "failed to set up buffer for position " + to_string(position);
    });
    return false;
}

}    // Namesapce IO.
}    // Namespace K.
