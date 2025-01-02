/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "SynchronizedState.h"

#include <K/Core/Log.h>

#include "LoopThreadState.h"

using std::mutex;
using std::shared_ptr;
using std::size_t;
using std::unique_lock;
using K::Core::IoBufferInterface;
using K::Core::Log;
using K::Core::RunLoop;
using K::Core::UniqueHandle;

namespace K {
namespace IO {

Connection::SynchronizedState::SynchronizedState(const shared_ptr<RunLoop> &runLoop, int bufferSizeConstraint)
        : runLoop_{runLoop},
          error_{false},
          eof_{false},
          ioReadPaused_{false},
          ioWritePaused_{false},
          bufferSizeConstraint_{bufferSizeConstraint},
          syncRequested_{false} {
    // Nop.
}

void Connection::SynchronizedState::SetRunLoopClientId(int id) {
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    runLoopClientId_ = id;
}    // ......................................................................................... critical section, end.

void Connection::SynchronizedState::Sync(LoopThreadState *loopThreadState) {
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    Log::Print(Log::Level::DebugDebug, this, [&]{ return "Sync()"; });
    syncRequested_ = false;
    if (error_) {
        if (!loopThreadState->error) {
            loopThreadState->handlerNeedsReadyRead  = true;
            loopThreadState->handlerNeedsReadyWrite = true;
            loopThreadState->error                  = Error::IO;
        }
    } else {
        Transfer(readQueue_, loopThreadState->readQueue, static_cast<size_t>(loopThreadState->bufferSizeConstraint));
        if (ioReadPaused_ && (readQueue_.PayloadSize() < static_cast<size_t>(bufferSizeConstraint_))) {
            loopThreadState->unpauseIORead = true;
            ioReadPaused_ = false;
        }

        Transfer(loopThreadState->writeQueue, writeQueue_, static_cast<size_t>(bufferSizeConstraint_));
        if (ioWritePaused_ && !writeQueue_.Empty()) {
            loopThreadState->unpauseIOWrite = true;
            ioWritePaused_ = false;
        }

        if (eof_ && readQueue_.Empty()) {
            if (!loopThreadState->eof) {
                loopThreadState->handlerNeedsReadyRead = true;
                loopThreadState->eof                   = true;
            }
        }
    }
}    // ......................................................................................... critical section, end.

bool Connection::SynchronizedState::OnDataRead(UniqueHandle<IoBufferInterface> buffer) {
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    if (!error_ && !eof_) {
        readQueue_.Put(std::move(buffer));
        RequestSync(critical);
        if (readQueue_.PayloadSize() >= static_cast<size_t>(bufferSizeConstraint_)) {
            ioReadPaused_ = true;
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}    // ......................................................................................... critical section, end.

UniqueHandle<IoBufferInterface> Connection::SynchronizedState::OnReadyWrite() {
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    UniqueHandle<IoBufferInterface> buffer;
    if (!error_) {
        buffer = writeQueue_.Get();
        if (buffer) {
            RequestSync(critical);
        } else {
            ioWritePaused_ = true;
        }
    }
    return buffer;
}    // ......................................................................................... critical section, end.

void Connection::SynchronizedState::OnIncompleteWrite(UniqueHandle<IoBufferInterface> buffer) {
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    writeQueue_.PutBack(std::move(buffer));
}    // ......................................................................................... critical section, end.

void Connection::SynchronizedState::OnCustomCall() {
    // Nop. We're not using the connection I/O custom call feature.
}

void Connection::SynchronizedState::OnEof() {
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    if (!error_ && !eof_) {
        eof_ = true;
        RequestSync(critical);
    }
}    // ......................................................................................... critical section, end.

void Connection::SynchronizedState::OnError() {
    unique_lock<mutex> critical{lock_};    // Critical section .........................................................
    if (!error_) {
        error_ = true;
        RequestSync(critical);
    }
}    // ......................................................................................... critical section, end.

// Expects lock to be held.
void Connection::SynchronizedState::RequestSync(unique_lock<mutex> &critical) {
    (void) critical;
    if (!syncRequested_) {
        if (runLoopClientId_) {
            runLoop_->RequestActivation(*runLoopClientId_, true);
        }
        syncRequested_ = true;
    }
}

}    // Namespace IO.
}    // Namespace K.
