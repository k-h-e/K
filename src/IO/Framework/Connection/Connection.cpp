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

#include <K/Core/Result.h>
#include <K/Core/Framework/RunLoop.h>
#include <K/IO/ConnectionIO.h>
#include <K/IO/IOTools.h>
#include "LoopThreadState.h"
#include "SynchronizedState.h"

using std::make_shared;
using std::make_unique;
using std::optional;
using std::shared_ptr;
using K::Core::Result;
using K::Core::Framework::RunLoop;
using K::IO::IOTools;

namespace K {
namespace IO {
namespace Framework {

Connection::Connection(optional<int> fd, int bufferSizeConstraint, const shared_ptr<Result> &resultAcceptor,
                       const shared_ptr<RunLoop> &runLoop, const shared_ptr<ConnectionIO> &connectionIO)
        : loopThreadState_(make_unique<LoopThreadState>(
              runLoop, make_shared<SynchronizedState>(runLoop, ValidateBufferSizeConstraint(bufferSizeConstraint)),
              ValidateBufferSizeConstraint(bufferSizeConstraint), connectionIO)),
          finalResultAcceptor_(resultAcceptor) {
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

    if (!fd_) {
        loopThreadState_->synchronizedState->OnError();
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
        finalResultAcceptor_->Set(success);
    }
}

void Connection::Register(NonBlockingStreamInterface::HandlerInterface *handler, int id) {
    if (handler) {
        loopThreadState_->handler              = handler;
        loopThreadState_->handlerAssociatedId  = id;
        loopThreadState_->newHandlerRegistered = true;
        loopThreadState_->RequestActivation();
    } else {
        loopThreadState_->handler             = nullptr;
        loopThreadState_->handlerAssociatedId = 0;
    }
}

void Connection::SetFinalResultAcceptor(const shared_ptr<Result> &resultAcceptor) {
    finalResultAcceptor_ = resultAcceptor;
}

int Connection::Read(void *outBuffer, int bufferSize) {
    int numRead = 0;
    if (!loopThreadState_->error) {
        numRead = loopThreadState_->readBuffer.Get(outBuffer, bufferSize);
    }

    if (numRead) {
        loopThreadState_->RequestActivation();
    } else {
        loopThreadState_->clientReadPaused = true;
    }
    return numRead;
}

int Connection::Write(const void *data, int dataSize) {
    int numWritten = 0;
    if (!loopThreadState_->error) {
        if (loopThreadState_->bufferSizeConstraint > loopThreadState_->writeBuffer.Size()) {
            int numToWrite = std::min(loopThreadState_->bufferSizeConstraint - loopThreadState_->writeBuffer.Size(),
                                      dataSize);
            if (numToWrite > 0) {    // Defensive.
                loopThreadState_->writeBuffer.Put(data, numToWrite);
                numWritten = numToWrite;
            }
        }
    }

    if (numWritten) {
        loopThreadState_->RequestActivation();
    } else {
        loopThreadState_->clientWritePaused = true;
    }
    return numWritten;
}

bool Connection::Good() const {
    return (!ErrorState() && !Eof());
}

bool Connection::ErrorState() const {
    return loopThreadState_->error;
}

bool Connection::Eof() const {
    return (loopThreadState_->eof && loopThreadState_->readBuffer.Empty());
}

void Connection::ClearEof() {
    loopThreadState_->newHandlerRegistered = true;    // Will cause ready read and ready write to be emitted.
    loopThreadState_->RequestActivation();
}

int Connection::ValidateBufferSizeConstraint(int bufferSizeConstraint) {
    return (bufferSizeConstraint > 0) ? bufferSizeConstraint : 1024;
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
