////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_FRAMEWORK_HTTPGET_H_
#define K_IO_FRAMEWORK_HTTPGET_H_

#include <memory>
#include <optional>
#include <string>
#include <K/Core/StreamHandlerInterface.h>
#include <K/Core/Framework/AsyncInStreamInterface.h>
#include <K/Core/Framework/RunLoop.h>

namespace K {
    namespace Core {
        namespace Framework {
            class RunLoop;
        }
    }
    namespace IO {
        class ConnectionIO;
        namespace Framework {
            class InteractionConnectionEndPoint;
        }
    }
}

namespace K {
namespace IO {
namespace Framework {

//! Performs an HTTP GET request.
class HttpGet : public virtual Core::Framework::AsyncInStreamInterface,
                private virtual Core::Framework::RunLoop::ClientInterface,
                private virtual Core::StreamHandlerInterface {
  public:
    HttpGet(const std::string &host, const std::string &resource,
            const std::shared_ptr<Core::Framework::RunLoop> &runLoop, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    HttpGet()                                = delete;
    HttpGet(const HttpGet &other)            = delete;
    HttpGet &operator=(const HttpGet &other) = delete;
    HttpGet(HttpGet &&other)                 = delete;
    HttpGet &operator=(HttpGet &&other)      = delete;
    ~HttpGet();
    
    bool ErrorState() const override;
    Error StreamError() const override;
    void Register(Core::StreamHandlerInterface *handler, int activationId) override;

  private:
    void Activate(bool deepActivation) override;
    void OnStreamData(int id, const void *data, int dataSize) override;
    void OnStreamEnteredErrorState(int id, Core::StreamInterface::Error error) override;
    void OnHeaderData(const void *data, int dataSize);
    void OnContentData(const void *data, int dataSize);
    bool ProcessHeaderLine();
    
    const std::shared_ptr<Core::Framework::RunLoop> runLoop_;
    int                                             runLoopClientId_;
    std::shared_ptr<InteractionConnectionEndPoint>  endPoint_;
    Core::StreamHandlerInterface                    *handler_;
    int                                             handlerActivationId_;
    std::string                                     line_;
    bool                                            receivingHeader_;
    int                                             numHeaderLines_;
    std::optional<int>                              numContentBytes_;
    int                                             numContentBytesDelivered_;
    Core::StreamInterface::Error                    error_;
    bool                                            signalError_;
    
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_HTTPGET_H_
