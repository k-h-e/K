////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/IO/Framework/HttpGet.h>

#include <K/Core/TextWriter.h>
#include <K/IO/Framework/InteractionConnectionEndPoint.h>
#include <K/IO/Framework/TcpConnection.h>

using std::make_shared;
using std::shared_ptr;
using std::string;
using K::Core::StreamInterface;
using K::Core::TextWriter;
using K::Core::Framework::RunLoop;
using K::IO::Framework::TcpConnection;

namespace K {
namespace IO {
namespace Framework {

HttpGet::HttpGet(const string &host, const shared_ptr<RunLoop> &runLoop, const shared_ptr<ConnectionIO> &connectionIO)
        : runLoop_{runLoop},
          handler_{nullptr},
          handlerActivationId_{0},
          error_{Error::None},
          signalError_{false} {
    runLoopClientId_ = runLoop_->AddClient(this);
    
    auto connection{make_shared<TcpConnection>(host, 80, runLoop, connectionIO)};
    endPoint_ = make_shared<InteractionConnectionEndPoint>(connection, runLoop);
    endPoint_->Register(this, 0);
    TextWriter writer{endPoint_};
    writer.Write("GET /echobot/repository/public/Herborn.ebx HTTP/1.1\r\n");
    writer.Write("Host: www.hergenrother.de\r\n");
    writer.Write("Accept: */*\r\n");
    writer.Write("\r\n");
}

HttpGet::~HttpGet() {
    endPoint_->Register(nullptr, 0);
    runLoop_->RemoveClient(runLoopClientId_);
}

bool HttpGet::ErrorState() const {
    return (error_ != Error::None);
}

StreamInterface::Error HttpGet::StreamError() const {
    return error_;
}

void HttpGet::Register(StreamHandlerInterface *handler, int activationId) {
    handler_             = handler;
    handlerActivationId_ = handler_ ? activationId : 0;
    
    if (error_ != Error::None) {
        signalError_ = true;
        runLoop_->RequestActivation(runLoopClientId_, false);
    }
}

void HttpGet::Activate(bool deepActivation) {
    if (signalError_) {
        signalError_ = false;
        if (handler_) {
            handler_->OnStreamEnteredErrorState(handlerActivationId_, error_);
        }
    }
}

void HttpGet::OnStreamData(int id, const void *data, int dataSize) {
    if (handler_) {
        handler_->OnStreamData(handlerActivationId_, data, dataSize);
    }
}
void HttpGet::OnStreamEnteredErrorState(int id, StreamInterface::Error error) {
    if (error_ == Error::None) {
        error_ = error;
        if (handler_) {
            handler_->OnStreamEnteredErrorState(handlerActivationId_, error_);
        }
    }
}
                 
}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
