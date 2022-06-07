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
#include <K/Core/Buffer.h>
#include <K/Core/ItemWriteInterface.h>
#include <K/Core/RingBuffer.h>
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
class InteractionConnectionEndPoint : public virtual Core::ItemWriteInterface,
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

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the end point.
     *
     *  The handler is expected to outlive the end point. It will not get called upon end point destruction.
     *
     *  \param activationId
     *  ID to be passed along with handler activations for the end point. Useful in case one wants to use one handler
     *  with multiple end points.
     */
    void Register(Core::StreamHandlerInterface *handler, int activationId);
    void WriteItem(const void *item, int itemSize) override;
    bool WriteFailed() const override;
    void ClearWriteFailed() override;

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
    bool                                                                 writeFailed_;
    bool                                                                 error_;
    bool                                                                 eof_;
    bool                                                                 signalError_;
    bool                                                                 signalEof_;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_WRITEBUFFER_H_
