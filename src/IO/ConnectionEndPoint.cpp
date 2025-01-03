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
          activationRequested_{false},
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
            RequestActivation();
        }
    }
}

int ConnectionEndPoint::WriteBlocking(const void *data, int dataSize) {
    assert (dataSize > 0);
    int numWritten { 0 };
    if (!error_) { 
        writeBuffer_.AppendFromMemory(data, dataSize);
        RequestActivation();
        numWritten = dataSize;

        if (writeBuffer_.DataSize() > pushThreshold) {
            PushOutgoing();
        }
    }

    return numWritten;
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

// ---

void ConnectionEndPoint::Activate(bool deepActivation) {
    (void) deepActivation;

    if (!error_) {
        PushOutgoing();    // Might call RequestActivation(). Call this before resetting requested flag to avoid
                           // unnecessary activations.
    }

    activationRequested_ = false;

    if (signalError_) {
        signalError_ = false;
        if (readyRead_) {
            RequestActivation();
        }
        if (handler_ && error_) {
            handler_->OnStreamError(*error_);
        }
    } else if (!error_) {
        if (readyRead_) {
            DispatchIncoming();
        }
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
    auto buffer = connection_->ReadNonBlocking();
    if (buffer) {
        RequestActivation();
        if (handler_) {
            handler_->OnRawStreamData(std::move(buffer));
        }
    } else {
        readyRead_ = false;
        if (connection_->ErrorState()) {
            error_       = connection_->StreamError();
            assert (error_.has_value());
            signalError_ = true;
            RequestActivation();
        }
    }
}

void ConnectionEndPoint::PushOutgoing() {
    if (!writeBuffer_.Empty()) {
        Put(writeBuffer_.Data(), writeBuffer_.DataSize(), writeQueue_, *ioBuffers_);
        writeBuffer_.Clear();
    }

    if (readyWrite_ && !writeQueue_.Empty()) {
        Transfer(writeQueue_, *connection_);
        if (!writeQueue_.Empty()) {
            readyWrite_ = false;
            if (connection_->ErrorState()) {
                error_       = connection_->StreamError();
                assert (error_.has_value());
                signalError_ = true;
                RequestActivation();
            }
        }
    }
}

void ConnectionEndPoint::RequestActivation() {
    if (!activationRequested_) {
        runLoop_->RequestActivation(runLoopClientId_, false);
        activationRequested_ = true;
    }
}

}    // Namespace IO.
}    // Namespace K.
