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

#include <K/Core/Log.h>
#include <K/Core/StringTools.h>
#include <K/Core/TextWriter.h>
#include <K/IO/Framework/InteractionConnectionEndPoint.h>
#include <K/IO/Framework/TcpConnection.h>

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;
using K::Core::Log;
using K::Core::StreamInterface;
using K::Core::StringTools;
using K::Core::TextWriter;
using K::Core::Framework::RunLoop;
using K::IO::Framework::TcpConnection;

namespace K {
namespace IO {
namespace Framework {

HttpGet::HttpGet(const string &host, const string &resource, const shared_ptr<RunLoop> &runLoop,
                 const shared_ptr<ConnectionIO> &connectionIO)
        : runLoop_{runLoop},
          handler_{nullptr},
          handlerActivationId_{0},
          receivingHeader_{true},
          numHeaderLines_{0},
          numContentBytesDelivered_{0},
          error_{Error::None},
          signalError_{false} {
    runLoopClientId_ = runLoop_->AddClient(this);
    
    auto connection{make_shared<TcpConnection>(host, 80, runLoop, connectionIO)};
    endPoint_ = make_shared<InteractionConnectionEndPoint>(connection, runLoop);
    endPoint_->Register(this, 0);
    TextWriter writer{endPoint_};
    writer.Write("GET ");
    writer.Write(resource);
    writer.Write(" HTTP/1.1\r\n");
    writer.Write("Host: ");
    writer.Write(host);
    writer.Write("\r\n");
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
    (void)id;
    if (error_ == Error::None) {
        if (receivingHeader_) {
            OnHeaderData(data, dataSize);
        } else {
            OnContentData(data, dataSize);
        }
    }
}

void HttpGet::OnStreamEnteredErrorState(int id, StreamInterface::Error error) {
    (void)id;
    assert (error != Error::None);
    
    if (error_ == Error::None) {
        if (error == Error::Eof) {
            if (!numContentBytes_ || (numContentBytesDelivered_ != *numContentBytes_)) {
                error = Error::IO;
            }
        }
        
        error_ = error;
        if (error_ == Error::Eof) {
            Log::Print(Log::Level::Debug, this, [&]{
                return "EOF, bytes_received=" + to_string(numContentBytesDelivered_);
            });
        } else {
            Log::Print(Log::Level::Error, this, [&]{
                return "error after receiving " + to_string(numContentBytesDelivered_) + " bytes!";
            });
        }
        if (handler_) {
            handler_->OnStreamEnteredErrorState(handlerActivationId_, error_);
        }
    }
}

void HttpGet::OnHeaderData(const void *data, int dataSize) {
    const uint8_t *ptr = static_cast<const uint8_t *>(data);
    for (int i = 0; i < dataSize; ++i) {
        char character = static_cast<char>(*ptr);
        if (character == '\n') {
            if (line_.empty()) {
                if (numContentBytes_) {
                    receivingHeader_ = false;
                    int numRemaining = dataSize - i - 1;
                    if (numRemaining) {
                        numContentBytesDelivered_ += numRemaining;
                        if (handler_) {
                            handler_->OnStreamData(handlerActivationId_, ptr + 1, numRemaining);
                        }
                    }
                } else {
                    OnStreamEnteredErrorState(0, Error::IO);
                }
                return;
            } else {
                if (!ProcessHeaderLine()) {
                    OnStreamEnteredErrorState(0, Error::IO);
                    return;
                }
            }
        } else if (character == '\r') {
            // Nop.
        } else {
            line_.push_back(character);
        }
        
        ++ptr;
    }
}

void HttpGet::OnContentData(const void *data, int dataSize) {
    numContentBytesDelivered_ += dataSize;
            
    Error error = Error::None;
    if (numContentBytesDelivered_ == *numContentBytes_) {
        error = Error::Eof;
    } else if (numContentBytesDelivered_ > *numContentBytes_) {
        error = Error::IO;
    }
    
    if (handler_) {
        handler_->OnStreamData(handlerActivationId_, data, dataSize);
    }
    
    if (error != Error::None) {
        OnStreamEnteredErrorState(handlerActivationId_, error);
    }
}

bool HttpGet::ProcessHeaderLine() {
    bool success = false;
    if (!numHeaderLines_) {
        vector<string> tokens = StringTools::Tokenize(line_, " ", false);
        if (tokens.size() == 3u) {
            if ((tokens[1] == "200") && (tokens[2] == "OK")) {
                success = true;
            }
        }
    } else {
        vector<string> tokens = StringTools::Tokenize(line_, ":", false);
        if (tokens.size() == 2u) {
            if (tokens[0] == "Content-Length") {
                int num;
                if (StringTools::Parse(tokens[1], &num) && (num >= 0)) {
                    numContentBytes_ = num;
                    success = true;
                }
            } else {
                success = true;
            }
        } else if (tokens.size() > 2u) {
            success = true;
        }
    }
    
    ++numHeaderLines_;
    line_.clear();
    
    return success;
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
