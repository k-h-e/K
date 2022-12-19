////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_FRAMEWORK_INTERACTIONCONNECTIONENDPOINT_H_
#define K_IO_FRAMEWORK_INTERACTIONCONNECTIONENDPOINT_H_

#include <memory>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/Buffer.h>
#include <K/Core/RingBuffer.h>
#include <K/Core/Framework/AsyncInStreamInterface.h>
#include <K/Core/Framework/RunLoop.h>
#include <K/Core/Framework/NonBlockingIOStreamInterface.h>

namespace K {
    namespace Core {
        class StreamHandlerInterface;
        namespace Framework {
            class NonBlockingIOStreamInterface;
            class RunLoop;
        }
    }
}

namespace K {
namespace IO {
namespace Framework {

//! Convenience endpoint for nonblocking connection streams that are operated in an interaction-like fashion.
class InteractionConnectionEndPoint : public virtual Core::Framework::AsyncInStreamInterface,
                                      public virtual Core::BlockingOutStreamInterface,
                                      private virtual Core::Framework::RunLoop::ClientInterface,
                                      private virtual Core::Framework::NonBlockingIOStreamInterface::HandlerInterface {
  public:
    InteractionConnectionEndPoint(const std::shared_ptr<Core::Framework::NonBlockingIOStreamInterface> &connection,
                                  const std::shared_ptr<Core::Framework::RunLoop> &runLoop);
    InteractionConnectionEndPoint()                                                      = delete;
    InteractionConnectionEndPoint(const InteractionConnectionEndPoint &other)            = delete;
    InteractionConnectionEndPoint &operator=(const InteractionConnectionEndPoint &other) = delete;
    InteractionConnectionEndPoint(InteractionConnectionEndPoint &&other)                 = delete;
    InteractionConnectionEndPoint &operator=(InteractionConnectionEndPoint &&other)      = delete;
    ~InteractionConnectionEndPoint();

    void Register(Core::StreamHandlerInterface *handler, int activationId) override;
    int WriteBlocking(const void *data, int dataSize) override;
    bool ErrorState() const override;
    Error StreamError() const override;
    void SetCloseResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) override;

  private:
    void Activate(bool deepActivation) override;
    void OnStreamReadyRead(int id) override;
    void OnStreamReadyWrite(int id) override;

    void DispatchIncoming();
    void PushOutgoing();

    const std::shared_ptr<Core::Framework::NonBlockingIOStreamInterface> connection_;
    const std::shared_ptr<Core::Framework::RunLoop>                      runLoop_;
    int                                                                  runLoopClientId_;
    Core::StreamHandlerInterface                                         *handler_;
    int                                                                  handlerActivationId_;
    Core::Buffer                                                         readBuffer_;
    bool                                                                 readyRead_;
    Core::RingBuffer                                                     writeBuffer_;
    bool                                                                 readyWrite_;
    Error                                                                error_;
    bool                                                                 signalError_;
    std::shared_ptr<Core::ResultAcceptor>                                closeResultAcceptor_;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_WRITEBUFFER_H_
