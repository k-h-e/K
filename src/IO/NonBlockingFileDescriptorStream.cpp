#include <K/IO/NonBlockingFileDescriptorStream.h>

#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/IO.h>

using std::shared_ptr;
using std::unique_lock;
using std::mutex;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

NonBlockingFileDescriptorStream::NonBlockingFileDescriptorStream(int fd, int bufferSizeThreshold,
                                                                 const shared_ptr<IO> &io)
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

NonBlockingFileDescriptorStream::~NonBlockingFileDescriptorStream() {
    io_->Unregister(fd_);
}

void NonBlockingFileDescriptorStream::Register(HandlerInterface *handler) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    handler_ = handler;
}    // ......................................................................................... critical section, end.

void NonBlockingFileDescriptorStream::Register(ReadHandlerInterface *handler) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

}    // ......................................................................................... critical section, end.

int NonBlockingFileDescriptorStream::Read(void *outBuffer, int bufferSize) {
    assert(bufferSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    int numRead = 0;

    if (!error_ && !eof_) {
        numRead = readBuffer_.Read(outBuffer, bufferSize);
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

int NonBlockingFileDescriptorStream::Write(const void *data, int dataSize) {
    assert(dataSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    int numWritten = 0;

    if (!error_) {
        if (writeBuffer_.Fill() < bufferSizeThreshold_) {
            numWritten = std::min(dataSize, bufferSizeThreshold_ - writeBuffer_.Fill());    // >= 1.
            writeBuffer_.Write(data, numWritten);
            if (canNotWrite_) {
                io_->SetClientCanWrite(fd_);
                canNotWrite_ = false;
            }
        }
    }

    return numWritten;
}    // ......................................................................................... critical section, end.

bool NonBlockingFileDescriptorStream::WriteItem(const void *item, int itemSize) {
    assert(itemSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return false;
}    // ......................................................................................... critical section, end.


bool NonBlockingFileDescriptorStream::EndOfStream() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return eof_ && (readBuffer_.Fill() == 0);
}    // ......................................................................................... critical section, end.

bool NonBlockingFileDescriptorStream::Error() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return error_;
}    // ......................................................................................... critical section, end.

bool NonBlockingFileDescriptorStream::OnDataRead(void *data, int dataSize) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_ && !eof_) {
        (void)readBuffer_.Write(data, dataSize);
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

void NonBlockingFileDescriptorStream::OnReadyWrite() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

}    // ......................................................................................... critical section, end.

void NonBlockingFileDescriptorStream::OnEof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    eof_ = true;
}    // ......................................................................................... critical section, end.

void NonBlockingFileDescriptorStream::OnError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    error_ = true;
}    // ......................................................................................... critical section, end.

}    // Namesapce IO.
}    // Namespace K.
