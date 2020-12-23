#include "SharedState.h"

#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/BufferedFileDescriptorStream.h>
#include <K/IO/IO.h>

using std::shared_ptr;
using std::unique_lock;
using std::mutex;
using std::to_string;
using K::Core::Log;
using K::IO::IO;

namespace K {
namespace IO {

BufferedFileDescriptorStream::SharedState::SharedState(int fd, int bufferSizeThreshold, const shared_ptr<IO> &io)
        : io_(io),
          fd_(fd),
          handler_(nullptr),
          bufferSizeThreshold_(bufferSizeThreshold),
          needToSignalReadyRead_(true),
          canNotRead_(false),
          canNotWrite_(true),
          eof_(false),
          error_(false) {
    if (!io_->Register(fd_, this)) {
        error_ = true;
    }
    else {
        io_->SetClientCanRead(fd_);
    }
}

BufferedFileDescriptorStream::SharedState::~SharedState() {
    io_->Unregister(fd_);
}

void BufferedFileDescriptorStream::SharedState::Register(HandlerInterface *handler) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    handler_ = handler;
}    // ......................................................................................... critical section, end.

int BufferedFileDescriptorStream::SharedState::Read(void *outBuffer, int bufferSize) {
    assert(bufferSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    int numRead = 0;

    if (!error_ && !eof_) {
        numRead = readBuffer_.Get(outBuffer, bufferSize);
        if (!numRead) {
            needToSignalReadyRead_ = true;
        }

        if (canNotRead_ && (readBuffer_.Fill() < bufferSizeThreshold_)) {
            io_->SetClientCanRead(fd_);
            canNotRead_ = false;
        }
    }

    return numRead;
}    // ......................................................................................... critical section, end.

int BufferedFileDescriptorStream::SharedState::Write(const void *data, int dataSize) {
    assert(dataSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    int numWritten = 0;

    if (!error_) {
        if (writeBuffer_.Fill() < bufferSizeThreshold_) {
            numWritten = std::min(dataSize, bufferSizeThreshold_ - writeBuffer_.Fill());    // >= 1.
            writeBuffer_.Put(data, numWritten);
            if (canNotWrite_) {
                io_->SetClientCanWrite(fd_);
                canNotWrite_ = false;
            }
        }
    }

    return numWritten;
}    // ......................................................................................... critical section, end.

bool BufferedFileDescriptorStream::SharedState::Eof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return eof_ && (readBuffer_.Fill() == 0);
}    // ......................................................................................... critical section, end.

bool BufferedFileDescriptorStream::SharedState::Error() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return error_;
}    // ......................................................................................... critical section, end.

bool BufferedFileDescriptorStream::SharedState::OnDataRead(const void *data, int dataSize) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_ && !eof_) {
        readBuffer_.Put(data, dataSize);
        Log::Print(Log::Level::Debug, this, [&]{ return "read_buffer_fill=" + to_string(readBuffer_.Fill()); });

        if (readBuffer_.Fill() >= bufferSizeThreshold_) {
            canNotRead_ = true;
        }

        if (needToSignalReadyRead_) {
            if (handler_) {
                handler_->OnReadyRead();
            }
            needToSignalReadyRead_ = false;
        }

        return !canNotRead_;
    }
    else {
        return false;
    }
}    // ......................................................................................... critical section, end.

int BufferedFileDescriptorStream::SharedState::OnReadyWrite(void *buffer, int bufferSize) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

}    // ......................................................................................... critical section, end.

void BufferedFileDescriptorStream::SharedState::OnIncompleteWrite(const void *unwrittenData,
                                                                      int unwrittenDataSize) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    writeBuffer_.PutBack(unwrittenData, unwrittenDataSize);
}    // ......................................................................................... critical section, end.

void BufferedFileDescriptorStream::SharedState::OnEof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_ && !eof_) {
        eof_ = true;
        if (handler_) {
            handler_->OnReadyRead();
        }
    }
}    // ......................................................................................... critical section, end.

void BufferedFileDescriptorStream::SharedState::OnError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        error_ = true;
        if (handler_) {
            handler_->OnReadyRead();
            handler_->OnReadyWrite();
        }
    }
}    // ......................................................................................... critical section, end.

}    // Namesapce IO.
}    // Namespace K.
