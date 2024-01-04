/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_CONNECTIONENDPOINT_H_
#define K_IO_CONNECTIONENDPOINT_H_

#include <memory>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/Buffer.h>
#include <K/Core/RingBuffer.h>
#include <K/Core/AsyncInStreamInterface.h>
#include <K/Core/RunLoop.h>
#include <K/IO/Connection.h>

namespace K {
    namespace Core {
        class RawStreamHandlerInterface;
    }
}

namespace K {
namespace IO {

//! Convenience endpoint for connections that are operated in a reactive fashion.
class ConnectionEndPoint : public virtual Core::AsyncInStreamInterface,
                           public virtual Core::BlockingOutStreamInterface,
                           private virtual Core::RunLoop::ClientInterface,
                           private virtual Connection::HandlerInterface {
  public:
    ConnectionEndPoint(const std::shared_ptr<Connection> &connection, const std::shared_ptr<Core::RunLoop> &runLoop);
    ConnectionEndPoint()                                           = delete;
    ConnectionEndPoint(const ConnectionEndPoint &other)            = delete;
    ConnectionEndPoint &operator=(const ConnectionEndPoint &other) = delete;
    ConnectionEndPoint(ConnectionEndPoint &&other)                 = delete;
    ConnectionEndPoint &operator=(ConnectionEndPoint &&other)      = delete;
    ~ConnectionEndPoint();

    void Register(Core::RawStreamHandlerInterface *handler) override;
    int WriteBlocking(const void *data, int dataSize) override;
    bool ErrorState() const override;
    std::optional<Error> StreamError() const override;
    void SetCloseResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) override;

  private:
    void Activate(bool deepActivation) override;
    void OnStreamReadyRead() override;
    void OnStreamReadyWrite() override;

    void DispatchIncoming();
    void PushOutgoing();

    const std::shared_ptr<Connection>     connection_;
    const std::shared_ptr<Core::RunLoop>  runLoop_;
    int                                   runLoopClientId_;
    Core::RawStreamHandlerInterface       *handler_;
    Core::Buffer                          readBuffer_;
    bool                                  readyRead_;
    Core::RingBuffer                      writeBuffer_;
    bool                                  readyWrite_;
    std::optional<Error>                  error_;
    bool                                  signalError_;
    std::shared_ptr<Core::ResultAcceptor> closeResultAcceptor_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONNECTIONENDPOINT_H_
