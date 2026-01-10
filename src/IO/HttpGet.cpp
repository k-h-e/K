/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/HttpGet.h>

#include <K/Core/IoBufferInterface.h>
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
using K::Core::IoBufferInterface;
using K::Core::IoBuffers;
using K::Core::IoBufferQueue;
using K::Core::Log;
using K::Core::RunLoop;
using K::Core::StreamInterface;
using K::Core::StringTools;
using K::Core::TextWriter;
using K::Core::UniqueHandle;
using K::IO::TcpConnection;

namespace K {
namespace IO {

HttpGet::HttpGet(const string &host, const string &resource, const shared_ptr<RunLoop> &runLoop,
                 const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<IoBuffers> &ioBuffers)
        : ioBuffers_{ioBuffers},
          runLoop_{runLoop},
          handler_{nullptr},
          receivingHeader_{true},
          numHeaderLines_{0},
          numContentBytesDelivered_{0},
          signalError_{false} {
    runLoopClientId_ = runLoop_->AddClient(*this);
    
    auto connection{make_shared<TcpConnection>(host, 80, runLoop, connectionIO)};
    endPoint_ = make_shared<ConnectionEndPoint>(connection, runLoop, ioBuffers);
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
    (void) deepActivation;
    
    if (!contentQueue_.Empty()) {
        auto buffer = contentQueue_.Get();
        if (!contentQueue_.Empty() || signalError_) {
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
        if (handler_) {
            handler_->OnRawStreamData(std::move(buffer));
        }
    } else if (signalError_) {
        signalError_ = false;
        if (handler_) {
            handler_->OnStreamError(*error_);
        }
    }
}

void HttpGet::OnRawStreamData(UniqueHandle<IoBufferInterface> buffer) {
    if (!error_) {
        if (receivingHeader_) {
            ProcessHeaderData(buffer);
        } else {
            ProcessContentData(buffer);
        }
    }
}

void HttpGet::OnStreamError(StreamInterface::Error error) {
    RaiseError(error);
}

void HttpGet::ProcessHeaderData(UniqueHandle<IoBufferInterface> &buffer) {
    const uint8_t *ptr = static_cast<const uint8_t *>(buffer->Content());
    for (int i = 0; i < buffer->Size(); ++i) {
        char character = static_cast<char>(*ptr);
        if (character == '\n') {
            if (line_.empty()) {
                if (numContentBytes_ && (*numContentBytes_ >= 0)) {
                    receivingHeader_ = false;
                    int numRemaining = buffer->Size() - i - 1;
                    if (numRemaining) {
                        Put(ptr + 1, numRemaining, contentQueue_, *ioBuffers_);
                        runLoop_->RequestActivation(runLoopClientId_, false);
                        FinishContentEnqueue(numRemaining);
                    } else {
                        if (*numContentBytes_ == 0) {
                            RaiseError(Error::Eof);
                        }
                    }
                } else {
                    RaiseError(Error::IO);
                }
                return;
            } else {
                if (!ProcessHeaderLine()) {
                    RaiseError(Error::IO);
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

void HttpGet::ProcessContentData(UniqueHandle<IoBufferInterface> &buffer) {
    int size { buffer->Size() };
    contentQueue_.Put(std::move(buffer));
    runLoop_->RequestActivation(runLoopClientId_, false);
    FinishContentEnqueue(size);
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
                if (StringTools::Parse(tokens[1], num) && (num >= 0)) {
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

void HttpGet::FinishContentEnqueue(int numBytes) {
    int numRemaining = *numContentBytes_ - numContentBytesDelivered_;
    
    numContentBytesDelivered_ += numBytes;
     
    if (numBytes == numRemaining) {
        RaiseError(Error::Eof);
    } else if (numBytes > numRemaining) {
        RaiseError(Error::IO);
    }
}

void HttpGet::RaiseError(StreamInterface::Error error) {
    if (!error_) {
        if (error == Error::Eof) {
            if (!numContentBytes_ || (numContentBytesDelivered_ != *numContentBytes_)) {
                error = Error::IO;
            }
        }
        
        error_       = error;
        signalError_ = true;
        runLoop_->RequestActivation(runLoopClientId_, false);
        
        if (*error_ == Error::Eof) {
            Log::Print(Log::Level::Debug, this, [&]{
                return "EOF, bytes_received=" + to_string(numContentBytesDelivered_);
            });
        } else {
            Log::Print(Log::Level::Error, this, [&]{
                return "error after receiving " + to_string(numContentBytesDelivered_) + " bytes!";
            });
        }
    }
}

}    // Namespace IO.
}    // Namespace K.
