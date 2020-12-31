#include "SharedState.h"

#include <cassert>
#include <algorithm>
#include <K/Core/Log.h>
#include <K/IO/BufferedConnection.h>
#include <K/IO/ConnectionIO.h>

using std::shared_ptr;
using std::unique_lock;
using std::mutex;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

BufferedConnection::SharedState::SharedState(int bufferSizeThreshold, const shared_ptr<ConnectionIO> &connectionIO)
        : connectionIO_(connectionIO),
          handler_(nullptr),
          handlerCalledInitially_(false),
          bufferSizeThreshold_(bufferSizeThreshold),
          canNotWrite_(true),
          eof_(false),
          error_(false) {
    // Nop.
}

void BufferedConnection::SharedState::SetError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    error_ = true;
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::Register(HandlerInterface *handler) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    handler_                = handler;
    handlerCalledInitially_ = false;
    if (handler_ && !error_) {
        connectionIO_->SetClientCanRead(this);
    }
}    // ......................................................................................... critical section, end.

bool BufferedConnection::SharedState::WriteItem(const void *item, int itemSize) {
    assert(itemSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    const uint8_t *data   = static_cast<const uint8_t *>(item);
    int           numLeft = itemSize;
    while (!error_) {
        if (!numLeft) {
            return true;
        }

        int bufferFill = writeBuffer_.Fill();
        if (bufferFill >= bufferSizeThreshold_) {
            writeCanContinue_.wait(critical);
        }
        else {
            int numToCopy = std::min(numLeft, bufferSizeThreshold_ - bufferFill);    // Will be > 0.
            writeBuffer_.Put(data, numToCopy);
            data    += numToCopy;
            numLeft -= numToCopy;
            if (canNotWrite_) {
                connectionIO_->SetClientCanWrite(this);
                canNotWrite_ = false;
            }
        }
    }

    return false;
}    // ......................................................................................... critical section, end.


bool BufferedConnection::SharedState::Eof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return eof_;
}    // ......................................................................................... critical section, end.

bool BufferedConnection::SharedState::Error() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return error_;
}    // ......................................................................................... critical section, end.

// ----

bool BufferedConnection::SharedState::OnDataRead(const void *data, int dataSize) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_ && !eof_) {
        if (handler_) {
            handler_->OnDataRead(data, dataSize);
            return true;
        }
    }

    return false;
}    // ......................................................................................... critical section, end.

int BufferedConnection::SharedState::OnReadyWrite(void *buffer, int bufferSize) {
    assert(bufferSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    int numWritten = writeBuffer_.Get(buffer, bufferSize);
    if (!numWritten) {
        canNotWrite_ = true;
    }
    else {
        writeCanContinue_.notify_all();
    }
    return numWritten;
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnIncompleteWrite(const void *unwrittenData, int unwrittenDataSize) {
    assert (unwrittenDataSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    writeBuffer_.PutBack(unwrittenData, unwrittenDataSize);
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnEof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_ && !eof_) {
        eof_ = true;
        if (handler_) {
            handler_->OnEof();
        }
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_) {
        error_ = true;
        if (handler_) {
            handler_->OnError();
        }
        writeCanContinue_.notify_all();
    }
}    // ......................................................................................... critical section, end.

// Ensures lock to be held.
void BufferedConnection::SharedState::EnsureHandlerCalledInitially() {
    if (handler_) {
        if (!handlerCalledInitially_) {
            if (eof_) {
                handler_->OnError();
            }

            if (error_) {
                handler_->OnError();
            }

            handlerCalledInitially_ = true;
        }
    }
}

}    // Namesapce IO.
}    // Namespace K.
