/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "SharedState.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include <K/Core/IoBuffers.h>
#include <K/Core/Log.h>
#include <K/IO/ConnectionIO.h>

using std::memcpy;
using std::mutex;
using std::optional;
using std::shared_ptr;
using std::size_t;
using std::to_string;
using std::unique_lock;

using K::Core::IoBufferInterface;
using K::Core::IoBuffers;
using K::Core::Log;
using K::Core::RawStreamHandlerInterface;
using K::Core::StreamInterface;
using K::Core::UniqueHandle;

namespace K {
namespace IO {
namespace Deprecated {

BufferedConnection::SharedState::SharedState(int bufferSizeThreshold, const shared_ptr<ConnectionIO> &connectionIO,
                                             const shared_ptr<IoBuffers> &ioBuffers)
        : connectionIO_{connectionIO},
          ioBuffers_{ioBuffers},
          handlerCalledInitially_{true},
          bufferSizeThreshold_{bufferSizeThreshold},
          canNotWrite_{true} {
    // Nop.
}

void BufferedConnection::SharedState::SetError() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    if (!error_) {
        error_ = Error::Unspecific;
        Log::Print(Log::Level::Warning, this, []{ return "error state was set"; });
    }
}    // ......................................................................................... critical section, end.

bool BufferedConnection::SharedState::Register(const shared_ptr<RawStreamHandlerInterface> &handler) {
    assert(handler);
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    if (!error_) {
        handler_                = handler;
        handlerCalledInitially_ = false;
        connectionIO_->RequestCustomCall(this);
        connectionIO_->SetClientCanRead(this);
        return true;
    } else {
        return false;
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::Unregister(const shared_ptr<RawStreamHandlerInterface> &handler) {
    assert(handler);
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    if (handler == handler_) {
        handler_.reset();
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::WriteItem(const void *item, int itemSize) {
    assert(itemSize > 0);
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    bool done { false };
    while (!done && !error_) {
        if (writeQueue_.PayloadSize() >= static_cast<size_t>(bufferSizeThreshold_)) {
            writeCanContinue_.wait(critical);
        } else {
            Put(item, itemSize, writeQueue_, *ioBuffers_);
            if (canNotWrite_) {
                connectionIO_->SetClientCanWrite(this);
                canNotWrite_ = false;
            }
            done = true;
        }
    }
}    // ......................................................................................... critical section, end.

bool BufferedConnection::SharedState::Error() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    return (error_.has_value());
}    // ......................................................................................... critical section, end.

optional<StreamInterface::Error> BufferedConnection::SharedState::StreamError() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    return error_;
}    // ......................................................................................... critical section, end.


// ----

bool BufferedConnection::SharedState::OnDataRead(UniqueHandle<IoBufferInterface> buffer) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_) {
        if (handler_) {
            handler_->OnRawStreamData(std::move(buffer));
            return true;
        }
    }

    return false;
}    // ......................................................................................... critical section, end.

UniqueHandle<IoBufferInterface> BufferedConnection::SharedState::OnReadyWrite() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    auto buffer = writeQueue_.Get();
    if (buffer) {
        writeCanContinue_.notify_all();
    } else {
        canNotWrite_ = true;
    }
    return buffer;
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnIncompleteWrite(UniqueHandle<IoBufferInterface> buffer) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    writeQueue_.PutBack(std::move(buffer));
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnCustomCall() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    Log::Print(Log::Level::Debug, this, []{ return "handling custom call"; });
    EnsureHandlerCalledInitially();
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnEof() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_) {
        Log::Print(Log::Level::Debug, this, []{ return "reached EOF"; });
        error_ = Error::Eof;
        if (handler_) {
            handler_->OnStreamError(*error_);
        }
    }
}    // ......................................................................................... critical section, end.

void BufferedConnection::SharedState::OnError() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    EnsureHandlerCalledInitially();
    if (!error_) {
        Log::Print(Log::Level::Warning, this, []{ return "entered error state"; });
        error_ = Error::IO;
        if (handler_) {
            handler_->OnStreamError(*error_);
        }
        writeCanContinue_.notify_all();
    }
}    // ......................................................................................... critical section, end.

// Ensures lock to be held.
void BufferedConnection::SharedState::EnsureHandlerCalledInitially() {
    if (handler_) {
        if (!handlerCalledInitially_) {
            if (error_) {
                handler_->OnStreamError(*error_);
            }

            handlerCalledInitially_ = true;
        }
    }
}

}    // Namespace Deprecated.
}    // Namesapce IO.
}    // Namespace K.
