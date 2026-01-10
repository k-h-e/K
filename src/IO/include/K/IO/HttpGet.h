/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_HTTPGET_H_
#define K_IO_HTTPGET_H_

#include <memory>
#include <optional>
#include <string>
#include <K/Core/AsyncInStreamInterface.h>
#include <K/Core/IoBufferQueue.h>
#include <K/Core/RawStreamHandlerInterface.h>
#include <K/Core/RunLoop.h>

namespace K {
    namespace Core {
        class IoBuffers;
    }
    namespace IO {
        class ConnectionEndPoint;
        class ConnectionIO;
    }
}

namespace K {
namespace IO {

//! Performs an HTTP GET request.
class HttpGet : public virtual Core::AsyncInStreamInterface,
                private virtual Core::RunLoop::ClientInterface,
                private virtual Core::RawStreamHandlerInterface {
  public:
    HttpGet(const std::string &host, const std::string &resource, const std::shared_ptr<Core::RunLoop> &runLoop,
            const std::shared_ptr<IO::ConnectionIO> &connectionIO, const std::shared_ptr<Core::IoBuffers> &ioBuffers);
    HttpGet()                                = delete;
    HttpGet(const HttpGet &other)            = delete;
    HttpGet &operator=(const HttpGet &other) = delete;
    HttpGet(HttpGet &&other)                 = delete;
    HttpGet &operator=(HttpGet &&other)      = delete;
    ~HttpGet();
    
    bool ErrorState() const override;
    std::optional<Error> StreamError() const override;
    void Register(Core::RawStreamHandlerInterface *handler) override;

  private:
    void Activate(bool deepActivation) override;
    void OnRawStreamData(Core::UniqueHandle<Core::IoBufferInterface> buffer) override;
    void OnStreamError(Core::StreamInterface::Error error) override;
    void ProcessHeaderData(Core::UniqueHandle<Core::IoBufferInterface> &buffer);
    void ProcessContentData(Core::UniqueHandle<Core::IoBufferInterface> &buffer);
    bool ProcessHeaderLine();
    void FinishContentEnqueue(int numBytes);
    void RaiseError(Core::StreamInterface::Error error);
    
    std::shared_ptr<Core::IoBuffers> ioBuffers_;    // Thread-safe.
    
    const std::shared_ptr<Core::RunLoop>        runLoop_;
    int                                         runLoopClientId_;
    std::shared_ptr<ConnectionEndPoint>         endPoint_;
    Core::RawStreamHandlerInterface             *handler_;
    std::string                                 line_;
    bool                                        receivingHeader_;
    int                                         numHeaderLines_;
    std::optional<int>                          numContentBytes_;
    int                                         numContentBytesDelivered_;
    Core::IoBufferQueue                         contentQueue_;
    std::optional<Core::StreamInterface::Error> error_;
    bool                                        signalError_;
    
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_HTTPGET_H_
