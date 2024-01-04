/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/HttpGet.h>

#include <K/Core/Log.h>
#include <K/Core/StringTools.h>
#include <K/Core/TextWriter.h>
#include <K/IO/ConnectionEndPoint.h>
#include <K/IO/TcpConnection.h>

using std::make_shared;
using std::optional;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;
using K::Core::Log;
using K::Core::RunLoop;
using K::Core::StreamInterface;
using K::Core::StringTools;
using K::Core::TextWriter;
using K::IO::TcpConnection;

namespace K {
namespace IO {

HttpGet::HttpGet(const string &host, const string &resource, const shared_ptr<RunLoop> &runLoop,
                 const shared_ptr<ConnectionIO> &connectionIO)
        : runLoop_{runLoop},
          handler_{nullptr},
          receivingHeader_{true},
          numHeaderLines_{0},
          numContentBytesDelivered_{0},
          signalError_{false} {
    runLoopClientId_ = runLoop_->AddClient(this);
    
    auto connection{make_shared<TcpConnection>(host, 80, runLoop, connectionIO)};
    endPoint_ = make_shared<ConnectionEndPoint>(connection, runLoop);
    endPoint_->Register(this);
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
    endPoint_->Register(nullptr);
    runLoop_->RemoveClient(runLoopClientId_);
}

bool HttpGet::ErrorState() const {
    return (error_.has_value());
}

optional<StreamInterface::Error> HttpGet::StreamError() const {
    return error_;
}

void HttpGet::Register(RawStreamHandlerInterface *handler) {
    handler_ = handler;

    if (error_) {
        signalError_ = true;
        runLoop_->RequestActivation(runLoopClientId_, false);
    }
}

void HttpGet::Activate(bool deepActivation) {
    if (signalError_) {
        signalError_ = false;
        if (handler_) {
            handler_->OnStreamError(*error_);
        }
    }
}

void HttpGet::OnRawStreamData(const void *data, int dataSize) {
    if (!error_) {
        if (receivingHeader_) {
            OnHeaderData(data, dataSize);
        } else {
            OnContentData(data, dataSize);
        }
    }
}

void HttpGet::OnStreamError(StreamInterface::Error error) {
    if (!error_) {
        if (error == Error::Eof) {
            if (!numContentBytes_ || (numContentBytesDelivered_ != *numContentBytes_)) {
                error = Error::IO;
            }
        }
        
        error_ = error;
        if (*error_ == Error::Eof) {
            Log::Print(Log::Level::Debug, this, [&]{
                return "EOF, bytes_received=" + to_string(numContentBytesDelivered_);
            });
        } else {
            Log::Print(Log::Level::Error, this, [&]{
                return "error after receiving " + to_string(numContentBytesDelivered_) + " bytes!";
            });
        }
        if (handler_) {
            handler_->OnStreamError(*error_);
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
                            handler_->OnRawStreamData(ptr + 1, numRemaining);
                        }
                    }
                } else {
                    OnStreamError(Error::IO);
                }
                return;
            } else {
                if (!ProcessHeaderLine()) {
                    OnStreamError(Error::IO);
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
            
    optional<Error> error;
    if (numContentBytesDelivered_ == *numContentBytes_) {
        error = Error::Eof;
    } else if (numContentBytesDelivered_ > *numContentBytes_) {
        error = Error::IO;
    }
    
    if (handler_) {
        handler_->OnRawStreamData(data, dataSize);
    }
    
    if (error) {
        OnStreamError(*error);
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

}    // Namespace IO.
}    // Namespace K.
