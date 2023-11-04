/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_NETWORKEVENTCOUPLING_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_H_

#include <memory>
#include <K/Core/Buffer.h>
#include <K/Core/ErrorStateInterface.h>
#include <K/Core/RingBuffer.h>
#include <K/Core/RunLoop.h>
#include <K/Core/StreamHandlerInterface.h>
#include <K/Core/Timer.h>
#include <K/Events/EventNotifier.h>

namespace K {
    namespace Core {
        class Timers;
    }
    namespace IO {
        class ConnectionEndPoint;
        class KeepAliveParameters;
        class TcpConnection;
    }
    namespace Events {
        class EventHub;
    }
}

namespace K {
namespace Events {

//! Extends the event mechanism to other nodes across the network.
class NetworkEventCoupling : public virtual K::Core::ErrorStateInterface,
                             private virtual Core::StreamHandlerInterface,
                             private virtual K::Core::Timer::HandlerInterface,
                             private virtual EventNotifier::HandlerInterface,
                             private virtual K::Core::RunLoop::ClientInterface {
  public:
    //! Interface to network event coupling handlers.
    class HandlerInterface : public virtual K::Core::Interface {
      public:
        //! Tells the handler that the network event coupling has entered error state.
        /*!
         *  \param id
         *  ID that was given when the handler was registered.
         */
        virtual void OnNetworkEventCouplingErrorState(int id) = 0;
    };

    NetworkEventCoupling(
        const std::shared_ptr<IO::TcpConnection> &tcpConnection, const std::string &protocolVersion,
        const IO::KeepAliveParameters &keepAliveParameters, const std::shared_ptr<EventHub> &hub,
        const std::shared_ptr<K::Core::RunLoop> &runLoop, const std::shared_ptr<K::Core::Timers> &timers);
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
     *
     *  \param id
     *  ID to be passed along with handler activations network event coupling. Useful in case one wants to use one
     *  handler with multiple network event couplings.
     */
    void Register(HandlerInterface *handler, int id);
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

    void OnStreamData(int id, const void *data, int dataSize) override;
    void OnStreamEnteredErrorState(int id, Core::StreamInterface::Error error) override;
    void OnTimer(int id) override;
    void OnEventsAvailable(int id) override;
    void Activate(bool deepActivation) override;

    void CopyDown();
    void SendVersionChunk();
    void SendEventsChunk(const void *data, int dataSize);
    void SendKeepAliveChunk();
    void EnterErrorState();

    const std::shared_ptr<EventHub>         hub_;              // Thread safe.

    std::unique_ptr<IO::ConnectionEndPoint> tcpConnection_;    // Present <=> not in error state.
    std::unique_ptr<K::Core::Timer>         timer_;            // Present <=> not in error state.
    std::unique_ptr<EventNotifier>          eventNotifier_;    // Present <=> not in error state.
    const std::shared_ptr<K::Core::RunLoop> runLoop_;
    int                                     runLoopClientId_;
    std::string                             protocolVersion_;
    int                                     numSendsBetweenKeepAliveChecks_;
    int                                     hubClientId_;
    NetworkEventCoupling::HandlerInterface  *handler_;
    int                                     handlerAssociatedId_;
    State                                   state_;
    K::Core::Buffer                         readBuffer_;
    int                                     readCursor_;
    int                                     readChunkSize_;
    std::unique_ptr<K::Core::Buffer>        eventBuffer_;
    bool                                    protocolVersionMatch_;
    int                                     numKeepAliveSendsUntilCheck_;
    bool                                    keepAliveReceived_;
    bool                                    error_;
    bool                                    signalErrorState_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_H_
