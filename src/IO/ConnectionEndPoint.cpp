/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/ConnectionEndPoint.h>

#include <cassert>

#include <K/Core/RawStreamHandlerInterface.h>
#include <K/Core/ResultAcceptor.h>

using std::optional;
using std::shared_ptr;

using K::Core::RawStreamHandlerInterface;
using K::Core::ResultAcceptor;
using K::Core::RunLoop;
using K::Core::StreamInterface;

namespace K {
namespace IO {

ConnectionEndPoint::ConnectionEndPoint(const shared_ptr<Connection> &connection, const shared_ptr<RunLoop> &runLoop)
        : connection_{connection},
          runLoop_{runLoop},
          handler_{nullptr},
          handlerActivationId_{0},
          readyRead_{false},
          readyWrite_{false},
          signalError_{false} {
    runLoopClientId_ = runLoop_->AddClient(this);
    connection_->Register(this, 0);
}

ConnectionEndPoint::~ConnectionEndPoint() {
    connection_->Register(nullptr, 0);
    runLoop_->RemoveClient(runLoopClientId_);

    if (closeResultAcceptor_) {
        if (!error_) {
            closeResultAcceptor_->OnSuccess();
        } else {
            closeResultAcceptor_->OnFailure();
        }
    }
}

void ConnectionEndPoint::Register(RawStreamHandlerInterface *handler, int activationId) {
    handler_             = handler;
    handlerActivationId_ = activationId;

    if (handler_) {
        if (error_) {
            signalError_ = true;
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
    }
}

int ConnectionEndPoint::WriteBlocking(const void *data, int dataSize) {
    assert (dataSize > 0);
    if (!error_) {
        writeBuffer_.Put(data, dataSize);
        PushOutgoing();
        if (!error_) {
            return dataSize;
        }
    }

    return 0;
}

bool ConnectionEndPoint::ErrorState() const {
    return (error_.has_value());
}

optional<StreamInterface::Error> ConnectionEndPoint::StreamError() const {
    return error_;
}

void ConnectionEndPoint::SetCloseResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    closeResultAcceptor_ = resultAcceptor;
}

void ConnectionEndPoint::Activate(bool deepActivation) {
    (void) deepActivation;

    if (signalError_) {
        signalError_ = false;
        if (readyRead_) {
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
        if (handler_ && error_) {
            handler_->OnStreamError(handlerActivationId_, *error_);
        }
    } else {
        DispatchIncoming();
    }
}

void ConnectionEndPoint::OnStreamReadyRead(int id) {
    (void) id;
    if (!error_) {
        readyRead_ = true;
        DispatchIncoming();
    }
}

void ConnectionEndPoint::OnStreamReadyWrite(int id) {
    (void) id;
    if (!error_) {
        readyWrite_ = true;
        PushOutgoing();
    }
}

void ConnectionEndPoint::DispatchIncoming() {
    if (!error_ && readyRead_) {
        readBuffer_.Clear();
        if (readBuffer_.AppendFromStream(connection_.get(), 2048)) {
            runLoop_->RequestActivation(runLoopClientId_, false);
            if (handler_) {
                handler_->OnRawStreamData(handlerActivationId_, readBuffer_.Data(), readBuffer_.DataSize());
            }
        } else {
            readyRead_ = false;
            if (connection_->ErrorState()) {
                error_       = connection_->StreamError();
                assert (error_.has_value());
                signalError_ = true;
                runLoop_->RequestActivation(runLoopClientId_, false);
            }
        }
    }
}

void ConnectionEndPoint::PushOutgoing() {
    if (!error_ && readyWrite_ && !writeBuffer_.Empty()) {
        writeBuffer_.TransferTo(connection_.get());
        if (!writeBuffer_.Empty()) {
             readyWrite_ = false;
             if (connection_->ErrorState()) {
                 error_       = connection_->StreamError();
                assert (error_.has_value());
                 signalError_ = true;
                 runLoop_->RequestActivation(runLoopClientId_, false);
             }
        }
    }
}

}    // Namespace IO.
}    // Namespace K.
