/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_NETWORKEVENTCOUPLING_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_H_

#include <memory>

#include <K/Core/Buffer.h>
#include <K/Core/ErrorStateInterface.h>
#include <K/Core/RawStreamHandlerInterface.h>
#include <K/Core/RunLoop.h>
#include <K/Core/Timer.h>
#include <K/Events/EventNotifier.h>

namespace K {
    namespace Core {
        class IoBuffers;
        class Timers;
    }
    namespace IO {
        class ConnectionEndPoint;
        class KeepAliveParameters;
        class TcpConnection;
    }
    namespace Events {
        class Event;
        class EventHub;
    }
}

namespace K {
namespace Events {

//! Extends the event mechanism to other nodes across the network.
class NetworkEventCoupling : public virtual Core::ErrorStateInterface,
                             private virtual Core::RawStreamHandlerInterface,
                             private virtual Core::Timer::HandlerInterface,
                             private virtual EventNotifier::HandlerInterface,
                             private virtual Core::RunLoop::ClientInterface {
  public:
    //! Interface to network event coupling handlers.
    class HandlerInterface : public virtual Core::Interface {
      public:
        //! Tells the handler that the network event coupling has entered error state.
        virtual void OnNetworkEventCouplingErrorState() = 0;
    };

    NetworkEventCoupling(
        const std::shared_ptr<IO::TcpConnection> &tcpConnection, const std::string &protocolVersion,
        const IO::KeepAliveParameters &keepAliveParameters, const std::shared_ptr<EventHub> &hub,
        const std::shared_ptr<Event> &connectedEvent, const std::shared_ptr<Event> &disconnectedEvent,
        const std::shared_ptr<Core::RunLoop> &runLoop, const std::shared_ptr<Core::IoBuffers> &ioBuffers,
        const std::shared_ptr<Core::Timers> &timers);
    NetworkEventCoupling()                                             = delete;
    NetworkEventCoupling(const NetworkEventCoupling &other)            = delete;
    NetworkEventCoupling &operator=(const NetworkEventCoupling &other) = delete;
    NetworkEventCoupling(NetworkEventCoupling &&other)                 = delete;
    NetworkEventCoupling &operator=(NetworkEventCoupling &&other)      = delete;
    ~NetworkEventCoupling();

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the network
     *  event coupling.
     *
     *  The handler is expected to outlive the network event coupling.
     */
    void Register(HandlerInterface *handler);
    bool ErrorState() const override;

  private:
    enum class ChunkType : uint16_t {
        KeepAlive,
        Events,
        Version
    };
    enum class State { AcceptingChunkSize,
                       AcceptingChunkData
    };

    void OnRawStreamData(Core::UniqueHandle<Core::IoBufferInterface> buffer) override;
    void OnStreamError(Core::StreamInterface::Error error) override;
    void OnTimer() override;
    void OnEventsAvailable() override;
    void Activate(bool deepActivation) override;

    void CopyDown();
    void FilterEvents(Core::BlockingInStreamInterface &stream);
    void SendVersionChunk();
    void SendEventsChunk(const void *data, int dataSize);
    void SendKeepAliveChunk();
    void EnterErrorState();
    void EnsureDisconnectedEventPosted();

    const std::shared_ptr<EventHub>         hub_;              // Thread safe.

    std::unique_ptr<IO::ConnectionEndPoint> tcpConnection_;    // Present <=> not in error state.
    std::unique_ptr<Core::Timer>            timer_;            // Present <=> not in error state.
    std::unique_ptr<EventNotifier>          eventNotifier_;    // Present <=> not in error state.
    const std::shared_ptr<Core::RunLoop>    runLoop_;
    int                                     runLoopClientId_;
    std::string                             protocolVersion_;
    int                                     numSendsBetweenKeepAliveChecks_;
    int                                     hubClientId_;
    NetworkEventCoupling::HandlerInterface  *handler_;
    std::shared_ptr<Event>                  connectedEvent_;
    std::shared_ptr<Event>                  disconnectedEvent_;
    State                                   state_;
    Core::Buffer                            readBuffer_;
    int                                     readCursor_;
    int                                     readChunkSize_;
    std::unique_ptr<Core::Buffer>           eventBuffer_;
    bool                                    protocolVersionMatch_;
    int                                     numKeepAliveSendsUntilCheck_;
    bool                                    keepAliveReceived_;
    bool                                    error_;
    bool                                    signalErrorState_;
    bool                                    disconnectedEventPosted_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_H_
