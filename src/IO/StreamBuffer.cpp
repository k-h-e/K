#include <K/IO/StreamBuffer.h>

#include <cassert>
#include <cstring>
#include <K/Core/Result.h>
#include <K/Core/Log.h>
#include <K/IO/IOTools.h>

using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::to_string;
using K::Core::Result;
using K::Core::Log;

namespace K {
namespace IO {

StreamBuffer::StreamBuffer(const shared_ptr<SeekableBlockingIOStreamInterface> &stream, File::AccessMode accessMode,
                           int bufferSize)
        : StreamBuffer(stream, accessMode, bufferSize, nullptr) {
    // Nop.
}

StreamBuffer::StreamBuffer(const shared_ptr<SeekableBlockingIOStreamInterface> &stream, File::AccessMode accessMode,
                           int bufferSize, const shared_ptr<Result> &resultAcceptor)
        : stream_(stream),
          bufferPosition_(0),
          cursor_(0),
          fill_(0),
          dirty_(false),
          readFailed_(false),
          writeFailed_(false),
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

    Result finalResult;
    if (error_) {
        finalResult.Set(false);
    }
    else {
        if (streamResult->Success()) {
            finalResult.Set(true);
        }
        else if (streamResult->Failure()) {
            finalResult.Set(false);
        }
    }
    if (finalResultAcceptor_) {
        *finalResultAcceptor_ = finalResult;
    }
    Log::Print(Log::Level::Debug, this, [&]{
        return "closed, final_result=" + finalResult.ToString();
    });
}

int StreamBuffer::ReadBlocking(void *buffer, int bufferSize) {
    assert(bufferSize > 0);
    int numRead = 0;
    if (!readFailed_) {
        if (!error_ && !eof_) {
            if (!readable_) {
                error_ = true;
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
                        if (!error_) {
                            numRead = numToRead;
                        } else {
                            Log::Print(Log::Level::Error, this, [&]{ return "error while reading"; });
                        }
                    } else {
                        cursor_ = newCursor;
                        numRead = numToRead;
                    }
                } else {
                    eof_ = true;
                }
            }
        }
    }

    if (!numRead) {
        readFailed_ = true;
    }
    return numRead;
}

bool StreamBuffer::ReadFailed() const {
    return readFailed_;
}

void StreamBuffer::ClearReadFailed() {
    readFailed_ = false;
}

int StreamBuffer::WriteBlocking(const void *data, int dataSize) {
    assert(dataSize > 0);
    int numWritten = 0;
    if (!writeFailed_) {
        if (!error_) {
            if (!writable_) {
                error_ = true;
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
                    if (!error_) {
                        numWritten = numToWrite;
                    } else {
                        error_ = true;
                        Log::Print(Log::Level::Error, this, []{ return "error while writing"; });
                    }
                }
                else {
                    cursor_    = newCursor;
                    numWritten = numToWrite;
                }
            }
        }
    }

    if (!numWritten) {
        writeFailed_ = true;
    }
    return numWritten;
}

bool StreamBuffer::WriteFailed() const {
    return writeFailed_;
}

void StreamBuffer::ClearWriteFailed() {
    writeFailed_ = false;
}

void StreamBuffer::Seek(int64_t position) {
    if (!error_) {
        if (position >= 0) {
            int newBufferPosition = BufferPositionFor(position);
            if (newBufferPosition != bufferPosition_) {
                if (Flush()) {
                    if (SetUpBuffer(position)) {
                        return;
                    }
                }
            }
            else {
                cursor_ = position - bufferPosition_;
                return;
            }
        }

        error_ = true;
        Log::Print(Log::Level::Error, this, [&]{
            return "error while seeking to position " + to_string(position); });
    }
}

int64_t StreamBuffer::StreamPosition() const {
    return bufferPosition_ + static_cast<int64_t>(cursor_);
}

bool StreamBuffer::Eof() const {
    return (eof_ && !error_);
}

bool StreamBuffer::ErrorState() const {
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
        int numReadTotal = 0;
        stream_->Seek(newBufferPosition);
        stream_->ClearReadFailed();
        while (!stream_->ReadFailed()) {
            numReadTotal += stream_->ReadBlocking(&buffer_[numReadTotal], bufferSize_ - numReadTotal);
            if ((numReadTotal == bufferSize_) || stream_->Eof()) {
                if (numReadTotal < bufferSize_) {
                    std::memset(&buffer_[numReadTotal], 0, bufferSize_ - numReadTotal);
                }
                bufferPosition_ = newBufferPosition;
                fill_           = numReadTotal;
                cursor_         = position - newBufferPosition;
                dirty_          = false;
                return true;
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
    stream_->Seek(bufferPosition_ + cursor);
    stream_->WriteItem(&buffer_[cursor], numBytes);
    if (!stream_->WriteFailed()) {
        return true;
    }
    else {
        Log::Print(Log::Level::Error, this, [&]{
            return ("failed to flush " + to_string(numBytes) + " dirty bytes to position "
                + to_string(bufferPosition_ + cursor));
        });
        return false;
    }
}

}    // Namesapce IO.
}    // Namespace K.
