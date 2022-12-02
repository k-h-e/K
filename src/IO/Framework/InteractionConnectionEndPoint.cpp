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

#include <cassert>
#include <K/Core/ResultAcceptor.h>
#include <K/Core/StreamHandlerInterface.h>

using std::shared_ptr;
using K::Core::ResultAcceptor;
using K::Core::StreamHandlerInterface;
using K::Core::StreamInterface;
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
          error_{Error::None},
          signalError_{false} {
    runLoopClientId_ = runLoop_->AddClient(this);
    connection_->Register(this, 0);
}

InteractionConnectionEndPoint::~InteractionConnectionEndPoint() {
    connection_->Register(nullptr, 0);
    runLoop_->RemoveClient(runLoopClientId_);

    if (closeResultAcceptor_) {
        if (error_ == Error::None) {
            closeResultAcceptor_->OnSuccess();
        } else {
            closeResultAcceptor_->OnFailure();
        }
    }
}

void InteractionConnectionEndPoint::Register(StreamHandlerInterface *handler, int activationId) {
    handler_             = handler;
    handlerActivationId_ = activationId;

    if (handler_) {
        if (error_ != Error::None) {
            signalError_ = true;
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
    }
}

int InteractionConnectionEndPoint::WriteBlocking(const void *data, int dataSize) {
    assert (dataSize > 0);
    if (error_ == Error::None) {
        writeBuffer_.Put(data, dataSize);
        PushOutgoing();
        if (error_ == Error::None) {
            return dataSize;
        }
    }

    return 0;
}

bool InteractionConnectionEndPoint::ErrorState() const {
    return (error_ != Error::None);
}

StreamInterface::Error InteractionConnectionEndPoint::StreamError() const {
    return error_;
}

void InteractionConnectionEndPoint::SetCloseResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    closeResultAcceptor_ = resultAcceptor;
}

void InteractionConnectionEndPoint::Activate(bool deepActivation) {
    (void)deepActivation;

    if (signalError_) {
        signalError_ = false;
        if (readyRead_) {
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
        if (handler_) {
            handler_->OnStreamEnteredErrorState(handlerActivationId_, error_);
        }
    } else {
        DispatchIncoming();
    }
}

void InteractionConnectionEndPoint::OnStreamReadyRead(int id) {
    (void)id;
    if (error_ == Error::None) {
        readyRead_ = true;
        DispatchIncoming();
    }
}

void InteractionConnectionEndPoint::OnStreamReadyWrite(int id) {
    (void)id;
    if (error_ == Error::None) {
        readyWrite_ = true;
        PushOutgoing();
    }
}

void InteractionConnectionEndPoint::DispatchIncoming() {
    if ((error_ == Error::None) && readyRead_) {
        readBuffer_.Clear();
        if (readBuffer_.AppendFromStream(connection_.get(), 2048)) {
            runLoop_->RequestActivation(runLoopClientId_, false);
            if (handler_) {
                handler_->OnStreamData(handlerActivationId_, readBuffer_.Data(), readBuffer_.DataSize());
            }
        } else {
            readyRead_ = false;
            if (connection_->ErrorState()) {
                error_       = connection_->StreamError();
                assert(error_ != Error::None);
                signalError_ = true;
                runLoop_->RequestActivation(runLoopClientId_, false);
            }
        }
    }
}

void InteractionConnectionEndPoint::PushOutgoing() {
    if ((error_ == Error::None) && readyWrite_ && !writeBuffer_.Empty()) {
        writeBuffer_.TransferTo(connection_.get());
        if (!writeBuffer_.Empty()) {
             readyWrite_ = false;
             if (connection_->ErrorState()) {
                 error_       = connection_->StreamError();
                 assert(error_ != Error::None);
                 signalError_ = true;
                 runLoop_->RequestActivation(runLoopClientId_, false);
             }
        }
    }
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
