/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/ConnectionEndPoint.h>

#include <cassert>
#include <cstring>

#include <K/Core/IoBuffers.h>
#include <K/Core/RawStreamHandlerInterface.h>
#include <K/Core/ResultAcceptor.h>

using std::memcpy;
using std::optional;
using std::shared_ptr;
using std::size_t;

using K::Core::IoBuffers;
using K::Core::RawStreamHandlerInterface;
using K::Core::ResultAcceptor;
using K::Core::RunLoop;
using K::Core::StreamInterface;

namespace K {
namespace IO {

ConnectionEndPoint::ConnectionEndPoint(const shared_ptr<Connection> &connection, const shared_ptr<RunLoop> &runLoop,
                                       const shared_ptr<IoBuffers> &ioBuffers)
        : ioBuffers_{ioBuffers},
          connection_{connection},
          runLoop_{runLoop},
          handler_{nullptr},
          readyRead_{false},
          readyWrite_{false},
          signalError_{false} {
    runLoopClientId_ = runLoop_->AddClient(this);
    connection_->Register(this);
}

ConnectionEndPoint::~ConnectionEndPoint() {
    connection_->Register(nullptr);
    runLoop_->RemoveClient(runLoopClientId_);

    if (closeResultAcceptor_) {
        if (!error_) {
            closeResultAcceptor_->OnSuccess();
        } else {
            closeResultAcceptor_->OnFailure();
        }
    }
}

void ConnectionEndPoint::Register(RawStreamHandlerInterface *handler) {
    handler_ = handler;

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
        Put(data, dataSize, writeQueue_, *ioBuffers_);
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
            handler_->OnStreamError(*error_);
        }
    } else {
        DispatchIncoming();
    }
}

void ConnectionEndPoint::OnStreamReadyRead() {
    if (!error_) {
        readyRead_ = true;
        DispatchIncoming();
    }
}

void ConnectionEndPoint::OnStreamReadyWrite() {
    if (!error_) {
        readyWrite_ = true;
        PushOutgoing();
    }
}

void ConnectionEndPoint::DispatchIncoming() {
    if (!error_ && readyRead_) {
        auto buffer = connection_->ReadNonBlocking();
        if (buffer) {
            runLoop_->RequestActivation(runLoopClientId_, false);
            if (handler_) {
                handler_->OnRawStreamData(std::move(buffer));
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
    if (!error_ && readyWrite_ && !writeQueue_.Empty()) {
        Transfer(writeQueue_, *connection_);
        if (!writeQueue_.Empty()) {
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
