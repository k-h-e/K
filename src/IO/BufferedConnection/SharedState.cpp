/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "SharedState.h"

#include <cassert>
#include <algorithm>

#include <K/Core/Log.h>
#include <K/IO/BufferedConnection.h>
#include <K/IO/ConnectionIO.h>

using std::mutex;
using std::optional;
using std::shared_ptr;
using std::to_string;
using std::unique_lock;

using K::Core::Log;
using K::Core::StreamHandlerInterface;
using K::Core::StreamInterface;

namespace K {
namespace IO {

BufferedConnection::SharedState::SharedState(int bufferSizeThreshold, const shared_ptr<ConnectionIO> &connectionIO)
        : connectionIO_(connectionIO),
          handlerActivationId_(0),
          handlerCalledInitially_(true),
          bufferSizeThreshold_(bufferSizeThreshold),
          canNotWrite_(true) {
    // Nop.
}

void BufferedConnection::SharedState::SetError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        error_ = Error::Unspecific;
        Log::Print(Log::Level::Warning, this, []{ return "error state was set"; });
    }
}    // ......................................................................................... critical section, end.

bool BufferedConnection::SharedState::Register(const shared_ptr<StreamHandlerInterface> &handler, int activationId) {
    assert(handler);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        handler_                = handler;
        handlerActivationId_    = activationId;
        handlerCalledInitially_ = false;
        connectionIO_->RequestCustomCall(this);
        connectionIO_->SetClientCanRead(this);
        return true;
    } else {
        return false;
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::Unregister(const shared_ptr<StreamHandlerInterface> &handler) {
    assert(handler);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (handler == handler_) {
        handler_.reset();
        handlerActivationId_ = 0;
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::WriteItem(const void *item, int itemSize) {
    assert(itemSize > 0);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    const uint8_t *data   = static_cast<const uint8_t *>(item);
    int           numLeft = itemSize;
    while ((!error_) && numLeft) {
        int bufferFill = writeBuffer_.Size();
        if (bufferFill >= bufferSizeThreshold_) {
            writeCanContinue_.wait(critical);
        } else {
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

bool BufferedConnection::SharedState::Error() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return (error_.has_value());
}    // ......................................................................................... critical section, end.

optional<StreamInterface::Error> BufferedConnection::SharedState::StreamError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return error_;
}    // ......................................................................................... critical section, end.


// ----

bool BufferedConnection::SharedState::OnDataRead(const void *data, int dataSize) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_) {
        if (handler_) {
            handler_->OnStreamData(handlerActivationId_, data, dataSize);
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
    } else {
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
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnEof() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_) {
        Log::Print(Log::Level::Debug, this, []{ return "reached EOF"; });
        error_ = Error::Eof;
        if (handler_) {
            handler_->OnStreamEnteredErrorState(handlerActivationId_, *error_);
        }
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnError() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_) {
        Log::Print(Log::Level::Warning, this, []{ return "entered error state"; });
        error_ = Error::IO;
        if (handler_) {
            handler_->OnStreamEnteredErrorState(handlerActivationId_, *error_);
        }
        writeCanContinue_.notify_all();
    }
}    // ......................................................................................... critical section, end.

// Ensures lock to be held.
void BufferedConnection::SharedState::EnsureHandlerCalledInitially() {
    if (handler_) {
        if (!handlerCalledInitially_) {
            if (error_) {
                handler_->OnStreamEnteredErrorState(handlerActivationId_, *error_);
            }

            handlerCalledInitially_ = true;
        }
    }
}

}    // Namesapce IO.
}    // Namespace K.
