/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "SynchronizedState.h"

#include <K/Core/Log.h>
#include "LoopThreadState.h"

using std::mutex;
using std::shared_ptr;
using std::unique_lock;
using K::Core::Log;
using K::Core::RunLoop;

namespace K {
namespace IO {

Connection::SynchronizedState::SynchronizedState(const shared_ptr<RunLoop> &runLoop, int bufferSizeConstraint)
        : runLoop_(runLoop),
          error_(false),
          eof_(false),
          ioReadPaused_(false),
          ioWritePaused_(false),
          bufferSizeConstraint_(bufferSizeConstraint),
          syncRequested_(false) {
    // Nop.
}

void Connection::SynchronizedState::SetRunLoopClientId(int id) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    runLoopClientId_ = id;
}    // ......................................................................................... critical section, end.

void Connection::SynchronizedState::Sync(LoopThreadState *loopThreadState) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    Log::Print(Log::Level::DebugDebug, this, [&]{ return "Sync()"; });
    syncRequested_ = false;
    if (error_) {
        if (!loopThreadState->error) {
            loopThreadState->handlerNeedsReadyRead  = true;
            loopThreadState->handlerNeedsReadyWrite = true;
            loopThreadState->error                  = Error::IO;
        }
    } else {
        readBuffer_.TransferTo(&loopThreadState->readBuffer, loopThreadState->bufferSizeConstraint);
        if (ioReadPaused_ && (readBuffer_.Size() < bufferSizeConstraint_)) {
            loopThreadState->unpauseIORead = true;
            ioReadPaused_ = false;
        }

        loopThreadState->writeBuffer.TransferTo(&writeBuffer_, bufferSizeConstraint_);
        if (ioWritePaused_ && !writeBuffer_.Empty()) {
            loopThreadState->unpauseIOWrite = true;
            ioWritePaused_ = false;
        }

        if (eof_ && readBuffer_.Empty()) {
            if (!loopThreadState->eof) {
                loopThreadState->handlerNeedsReadyRead = true;
                loopThreadState->eof                   = true;
            }
        }
    }
}    // ......................................................................................... critical section, end.

bool Connection::SynchronizedState::OnDataRead(const void *data, int dataSize) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    if (!error_ && !eof_) {
        readBuffer_.Put(data, dataSize);
        RequestSync(critical);
        if (readBuffer_.Size() >= bufferSizeConstraint_) {
            ioReadPaused_ = true;
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}    // ......................................................................................... critical section, end.

int Connection::SynchronizedState::OnReadyWrite(void *buffer, int bufferSize) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    int numWritten = 0;
    if (!error_) {
        numWritten = writeBuffer_.Get(buffer, bufferSize);
        if (numWritten) {
            RequestSync(critical);
        } else {
            ioWritePaused_ = true;
        }
    }
    return numWritten;
}    // ......................................................................................... critical section, end.

void Connection::SynchronizedState::OnIncompleteWrite(const void *unwrittenData, int unwrittenDataSize) {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    writeBuffer_.PutBack(unwrittenData, unwrittenDataSize);
}    // ......................................................................................... critical section, end.

void Connection::SynchronizedState::OnCustomCall() {
    // Nop. We're not using the connection I/O custom call feature.
}

void Connection::SynchronizedState::OnEof() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    if (!error_ && !eof_) {
        eof_ = true;
        RequestSync(critical);
    }
}    // ......................................................................................... critical section, end.

void Connection::SynchronizedState::OnError() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    if (!error_) {
        error_ = true;
        RequestSync(critical);
    }
}    // ......................................................................................... critical section, end.

// Expects lock to be held.
void Connection::SynchronizedState::RequestSync(unique_lock<mutex> &critical) {
    (void)critical;
    if (!syncRequested_) {
        if (runLoopClientId_) {
            runLoop_->RequestActivation(*runLoopClientId_, true);
        }
        syncRequested_ = true;
    }
}

}    // Namespace IO.
}    // Namespace K.
