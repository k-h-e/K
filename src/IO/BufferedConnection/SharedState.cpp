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
          handlerCalledInitially_(true),
          bufferSizeThreshold_(bufferSizeThreshold),
          canNotWrite_(true),
          customCallReportEof_(false),
          eof_(false),
          error_(false) {
    // Nop.
}

void BufferedConnection::SharedState::SetError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        error_ = true;
        Log::Print(Log::Level::Warning, this, []{ return "error state was set"; });
    }
}    // ......................................................................................... critical section, end.

bool BufferedConnection::SharedState::Register(const shared_ptr<StreamHandlerInterface> &handler) {
    assert(handler);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        handler_                = handler;
        handlerCalledInitially_ = false;
        customCallReportEof_    = false;
        connectionIO_->RequestCustomCall(this);
        connectionIO_->SetClientCanRead(this);
        return true;
    }
    else {
        return false;
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::Unregister(const shared_ptr<StreamHandlerInterface> &handler) {
    assert(handler);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (handler == handler_) {
        handler_.reset();
        customCallReportEof_ = false;
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::WriteItem(const void *item, int itemSize) {
    assert(itemSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    const uint8_t *data   = static_cast<const uint8_t *>(item);
    int           numLeft = itemSize;
    while (!error_) {
        if (!numLeft) {
            return;
        }

        int bufferFill = writeBuffer_.Size();
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
}    // ......................................................................................... critical section, end.

bool BufferedConnection::SharedState::Good() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return !error_ && !eof_;
}    // ......................................................................................... critical section, end.


bool BufferedConnection::SharedState::Eof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return eof_;
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::ClearEof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (eof_) {
        customCallReportEof_ = true;
        connectionIO_->RequestCustomCall(this);
    }
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
            handler_->HandleStreamData(data, dataSize);
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
    EnsureHandlerCalledInitially();
    writeBuffer_.PutBack(unwrittenData, unwrittenDataSize);
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnCustomCall() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    Log::Print(Log::Level::Debug, this, []{ return "handling custom call"; });
    EnsureHandlerCalledInitially();
    if (customCallReportEof_) {
        customCallReportEof_ = false;
        if (handler_) {
            handler_->HandleEof();
        }
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnEof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_ && !eof_) {
        Log::Print(Log::Level::Debug, this, []{ return "reached EOF"; });
        eof_ = true;
        if (handler_) {
            handler_->HandleEof();
        }
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_) {
        Log::Print(Log::Level::Warning, this, []{ return "entered error state"; });
        error_ = true;
        if (handler_) {
            handler_->HandleError();
        }
        writeCanContinue_.notify_all();
    }
}    // ......................................................................................... critical section, end.

// Ensures lock to be held.
void BufferedConnection::SharedState::EnsureHandlerCalledInitially() {
    if (handler_) {
        if (!handlerCalledInitially_) {
            if (eof_) {
                handler_->HandleError();
            }

            if (error_) {
                handler_->HandleError();
            }

            handlerCalledInitially_ = true;
        }
    }
}

}    // Namesapce IO.
}    // Namespace K.
