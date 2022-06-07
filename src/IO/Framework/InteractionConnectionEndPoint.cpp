////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/IO/Framework/InteractionConnectionEndPoint.h>

#include <K/Core/StreamHandlerInterface.h>

using std::shared_ptr;
using K::Core::StreamHandlerInterface;
using K::Core::Framework::NonBlockingIOStreamInterface;
using K::Core::Framework::RunLoop;

namespace K {
namespace IO {
namespace Framework {

InteractionConnectionEndPoint::InteractionConnectionEndPoint(const shared_ptr<NonBlockingIOStreamInterface> &connection,
                                                             const shared_ptr<RunLoop> &runLoop)
        : connection_{connection},
          runLoop_{runLoop},
          handler_{nullptr},
          handlerActivationId_{0},
          readyRead_{false},
          readyWrite_{false},
          writeFailed_{false},
          error_{false},
          eof_{false},
          signalError_{false},
          signalEof_{false} {
    runLoopClientId_ = runLoop_->AddClient(this);
    connection_->Register(this, 0);
}

InteractionConnectionEndPoint::~InteractionConnectionEndPoint() {
    connection_->Register(nullptr, 0);
    runLoop_->RemoveClient(runLoopClientId_);
}

void InteractionConnectionEndPoint::Register(StreamHandlerInterface *handler, int activationId) {
    handler_             = handler;
    handlerActivationId_ = activationId;

    if (handler_) {
        if (error_) {
            signalError_ = true;
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
        if (eof_) {
            signalEof_ = true;
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
    }
}

void InteractionConnectionEndPoint::WriteItem(const void *item, int itemSize) {
    if (!writeFailed_) {
        if (error_) {
            writeFailed_ = true;
        } else {
            writeBuffer_.Put(item, itemSize);
            PushOutgoing();
            if (error_) {
                writeFailed_ = true;
            }
        }
    }
}

bool InteractionConnectionEndPoint::WriteFailed() const {
    return writeFailed_;
}

void InteractionConnectionEndPoint::ClearWriteFailed() {
    writeFailed_ = false;
}

void InteractionConnectionEndPoint::Activate(bool deepActivation) {
    (void)deepActivation;

    if (signalEof_) {
        signalEof_ = false;
        if (signalError_ || readyRead_) {
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
        if (handler_) {
            handler_->HandleEof(handlerActivationId_);
        }
    } else if (signalError_) {
        signalError_ = false;
        if (readyRead_) {
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
        if (handler_) {
            handler_->HandleError(handlerActivationId_);
        }
    } else {
        DispatchIncoming();
    }
}

void InteractionConnectionEndPoint::OnStreamReadyRead(int id) {
    (void)id;
    if (!error_ && !eof_) {
        readyRead_ = true;
        DispatchIncoming();
    }
}

void InteractionConnectionEndPoint::OnStreamReadyWrite(int id) {
    (void)id;
    if (!error_ && !eof_) {
        readyWrite_ = true;
        PushOutgoing();
    }
}

void InteractionConnectionEndPoint::DispatchIncoming() {
    if (!error_ && !eof_ && readyRead_) {
        readBuffer_.Clear();
        if (readBuffer_.AppendFromReader(connection_.get(), 2048)) {
            runLoop_->RequestActivation(runLoopClientId_, false);
            if (handler_) {
                handler_->HandleStreamData(handlerActivationId_, readBuffer_.Data(), readBuffer_.DataSize());
            }
        } else {
            readyRead_ = false;
            if (connection_->ErrorState()) {
                error_       = true;
                signalError_ = true;
                runLoop_->RequestActivation(runLoopClientId_, false);
            } else if (connection_->Eof()) {
                eof_       = true;
                signalEof_ = true;
                runLoop_->RequestActivation(runLoopClientId_, false);
            }
        }
    }
}

void InteractionConnectionEndPoint::PushOutgoing() {
    if (!error_ && !eof_ && readyWrite_ && !writeBuffer_.Empty()) {
        writeBuffer_.TransferTo(connection_.get());
        if (!writeBuffer_.Empty()) {
             readyWrite_ = false;
             if (connection_->ErrorState()) {
                 error_       = true;
                 signalError_ = true;
                 runLoop_->RequestActivation(runLoopClientId_, false);
             }
        }
    }
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
