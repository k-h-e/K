#include "SharedState.h"

#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/BufferedFileDescriptorConnection.h>
#include <K/IO/IO.h>

using std::shared_ptr;
using std::unique_lock;
using std::mutex;
using std::to_string;
using K::Core::Log;
using K::IO::IO;

namespace K {
namespace IO {

BufferedFileDescriptorConnection::SharedState::SharedState(int fd, int bufferSizeThreshold, const shared_ptr<IO> &io)
        : io_(io),
          fd_(fd),
          handler_(nullptr),
          bufferSizeThreshold_(bufferSizeThreshold),
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

BufferedFileDescriptorConnection::SharedState::~SharedState() {
    io_->Unregister(fd_);
}

void BufferedFileDescriptorConnection::SharedState::Register(HandlerInterface *handler) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    handler_ = handler;
}    // ......................................................................................... critical section, end.

bool BufferedFileDescriptorConnection::SharedState::WriteItem(const void *item, int itemSize) {
    assert(itemSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return false;
}    // ......................................................................................... critical section, end.


bool BufferedFileDescriptorConnection::SharedState::Eof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return eof_;
}    // ......................................................................................... critical section, end.

bool BufferedFileDescriptorConnection::SharedState::Error() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return error_;
}    // ......................................................................................... critical section, end.

bool BufferedFileDescriptorConnection::SharedState::OnDataRead(void *data, int dataSize) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_ && !eof_) {
        if (handler_) {
            handler_->OnDataRead(data, dataSize);
        }
        return true;
    }
    else {
        return false;
    }
}    // ......................................................................................... critical section, end.

void BufferedFileDescriptorConnection::SharedState::OnReadyWrite() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

}    // ......................................................................................... critical section, end.

void BufferedFileDescriptorConnection::SharedState::OnEof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_ && !eof_) {
        eof_ = true;
        if (handler_) {
            handler_->OnEof();
        }
    }
}    // ......................................................................................... critical section, end.

void BufferedFileDescriptorConnection::SharedState::OnError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        error_ = true;
        if (handler_) {
            handler_->OnError();
        }
    }
}    // ......................................................................................... critical section, end.

}    // Namesapce IO.
}    // Namespace K.
