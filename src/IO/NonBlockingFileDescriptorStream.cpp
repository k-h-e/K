#include <K/IO/NonBlockingFileDescriptorStream.h>

#include <K/IO/IO.h>

using std::shared_ptr;
using std::unique_lock;
using std::mutex;

namespace K {
namespace IO {

NonBlockingFileDescriptorStream::NonBlockingFileDescriptorStream(int fd, const shared_ptr<IO> &io)
        : io_(io),
          fd_(fd),
          eof_(false),
          error_(false) {
    if (!io_->Register(fd_, this, this)) {
        error_ = true;
    }
}

NonBlockingFileDescriptorStream::~NonBlockingFileDescriptorStream() {
    io_->Unregister(fd_);
}

int NonBlockingFileDescriptorStream::Read(void *outBuffer, int bufferSize) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    return 0;

}    // ......................................................................................... critical section, end.

int NonBlockingFileDescriptorStream::Write(const void *data, int dataSize) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    return 0;

}    // ......................................................................................... critical section, end.

bool NonBlockingFileDescriptorStream::EndOfStream() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    return false;

}    // ......................................................................................... critical section, end.

bool NonBlockingFileDescriptorStream::Error() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    return false;

}    // ......................................................................................... critical section, end.

void NonBlockingFileDescriptorStream::Register(HandlerInterface *handler) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

}    // ......................................................................................... critical section, end.

void NonBlockingFileDescriptorStream::OnReadyRead() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

}    // ......................................................................................... critical section, end.

void NonBlockingFileDescriptorStream::OnReadyWrite() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

}    // ......................................................................................... critical section, end.

}    // Namesapce IO.
}    // Namespace K.
