////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/IO/Framework/Connection.h>

#include <K/Core/Log.h>
#include <K/Core/ResultAcceptor.h>
#include <K/Core/Framework/RunLoop.h>
#include <K/IO/ConnectionIO.h>
#include <K/IO/IOTools.h>
#include "LoopThreadState.h"
#include "SynchronizedState.h"

using std::make_shared;
using std::make_unique;
using std::optional;
using std::shared_ptr;
using std::to_string;
using K::Core::Log;
using K::Core::ResultAcceptor;
using K::Core::Framework::RunLoop;
using K::IO::IOTools;

namespace K {
namespace IO {
namespace Framework {

Connection::Connection(optional<int> fd, int bufferSizeConstraint, const shared_ptr<RunLoop> &runLoop,
                       const shared_ptr<ConnectionIO> &connectionIO)
        : loopThreadState_(make_unique<LoopThreadState>(
              runLoop, make_shared<SynchronizedState>(runLoop, ValidateBufferSizeConstraint(bufferSizeConstraint)),
              ValidateBufferSizeConstraint(bufferSizeConstraint), connectionIO)) {
    loopThreadState_->runLoopClientId = loopThreadState_->runLoop->AddClient(loopThreadState_.get());
    loopThreadState_->synchronizedState->SetRunLoopClientId(loopThreadState_->runLoopClientId);

    if (fd) {
        if (loopThreadState_->connectionIO->Register(loopThreadState_->synchronizedState, *fd)) {
            fd_ = fd;
        } else {
            if (*fd >= 0) {
                (void)IOTools::CloseFileDescriptor(*fd, this);
            }
        }
    }

    if (fd_) {
        loopThreadState_->connectionIO->SetClientCanRead(loopThreadState_->synchronizedState.get());
        loopThreadState_->connectionIO->SetClientCanWrite(loopThreadState_->synchronizedState.get());
        Log::Print(Log::Level::Debug, this, [&]{
            return "connection established, fd=" + to_string(*fd_) + ", run_loop_client_id="
                + to_string(loopThreadState_->runLoopClientId);
        });
    } else {
        loopThreadState_->synchronizedState->OnError();
        Log::Print(Log::Level::Error, this, [&]{ return "connection created bad"; });
    }
}

Connection::~Connection() {
    bool success = false;

    // Note: We're currently not waiting for all remaining buffered write data to be written!

    if (fd_) {
        bool finalStreamError;
        loopThreadState_->connectionIO->Unregister(loopThreadState_->synchronizedState, &finalStreamError);
        bool closeError = !IOTools::CloseFileDescriptor(*fd_, this);
        if (!finalStreamError && !closeError) {
            success = true;
        }
    }

    loopThreadState_->runLoop->RemoveClient(loopThreadState_->runLoopClientId);

    if (finalResultAcceptor_) {
        if (success) {
            finalResultAcceptor_->OnSuccess();
        } else {
            finalResultAcceptor_->OnFailure();
        }
    }

    if (success) {
        Log::Print(Log::Level::Debug, this, [&]{ return "closed, fd=" + to_string(*fd_); });
    } else {
        Log::Print(Log::Level::Error, this, [&]{ return "error while closing"; });
    }
}

void Connection::Register(NonBlockingIOStreamInterface::HandlerInterface *handler, int id) {
    if (handler) {
        loopThreadState_->handler                = handler;
        loopThreadState_->handlerAssociatedId    = id;
        loopThreadState_->handlerNeedsReadyRead  = true;
        loopThreadState_->handlerNeedsReadyWrite = true;
        loopThreadState_->RequestActivation(false);
    } else {
        loopThreadState_->handler             = nullptr;
        loopThreadState_->handlerAssociatedId = 0;
    }
}

void Connection::SetFinalResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    finalResultAcceptor_ = resultAcceptor;
}

int Connection::ReadNonBlocking(void *buffer, int bufferSize) {
    int numRead = 0;
    if (!ErrorState() && !Eof()) {
        numRead = loopThreadState_->readBuffer.Get(buffer, bufferSize);
        if (numRead) {
            loopThreadState_->RequestActivation(true);
        } else {
            loopThreadState_->clientReadPaused = true;
        }
    }

    Log::Print(Log::Level::DebugDebug, this, [&]{ return "ReadNonBlocking(), num_read=" + to_string(numRead); });
    return numRead;
}

int Connection::WriteNonBlocking(const void *data, int dataSize) {
    int numWritten = 0;  
    if (!ErrorState()) {
        if (loopThreadState_->bufferSizeConstraint > loopThreadState_->writeBuffer.Size()) {
            int numToWrite = std::min(loopThreadState_->bufferSizeConstraint - loopThreadState_->writeBuffer.Size(),
                                      dataSize);
            if (numToWrite > 0) {    // Defensive.
                loopThreadState_->writeBuffer.Put(data, numToWrite);
                numWritten = numToWrite;
            }
        }

        if (numWritten) {
            loopThreadState_->RequestActivation(true);
        } else {
            loopThreadState_->clientWritePaused = true;
        }
    }

    Log::Print(Log::Level::DebugDebug, this, [&]{ return "WriteNonBlocking(), num_written=" + to_string(numWritten); });
    return numWritten;
}

bool Connection::ErrorState() const {
    return loopThreadState_->error;
}

bool Connection::Eof() const {
    return (loopThreadState_->eof && loopThreadState_->readBuffer.Empty() && !loopThreadState_->error);
}

int Connection::ValidateBufferSizeConstraint(int bufferSizeConstraint) {
    return (bufferSizeConstraint > 0) ? bufferSizeConstraint : 1024;
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
