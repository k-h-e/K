#ifndef K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLING_H_
#define K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLING_H_

#include <memory>
#include <string>
#include <vector>
#include <K/Core/ErrorStateInterface.h>
#include <K/IO/Framework/TcpConnection.h>

namespace K {
    namespace Events {
        class EventLoopHub;
    }
}

namespace K {
namespace Events {
namespace Framework {

//! Extends the event mechanism to other nodes across the network.
class NetworkEventCoupling : public virtual IO::Framework::TcpConnection::HandlerInterface,
                             public virtual Core::ErrorStateInterface {
  public:
    //! Interface to network event coupling handlers.
    class HandlerInterface : public virtual K::Core::Interface {
      public:
        //! Tells the handler that network event coupling has entered error state.
        /*!
         *  \param id
         *  ID that was given when the handler was registered.
         */
        virtual void OnNetworkEventCouplingErrorState(int id) = 0;
    };

    NetworkEventCoupling(
        std::unique_ptr<IO::Framework::TcpConnection> tcpConnection, const std::string &protocolVersion,
        const std::shared_ptr<EventLoopHub> &hub);
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
    void OnStreamReadyRead(int id) override;
    void OnStreamReadyWrite(int id) override;
    bool ErrorState() const override;

  private:
    enum class ChunkType : uint16_t {
        KeepAlive,
        Events,
        Version
    };

    const std::shared_ptr<EventLoopHub>                 hub_;                // Thread safe.

    const std::unique_ptr<IO::Framework::TcpConnection> tcpConnection_;
    std::string                                         protocolVersion_;
    HandlerInterface                                    *handler_;
    int                                                 handlerAssociatedId_;
    std::vector<uint8_t>                                readBuffer_;
    int                                                 readBufferFill_;
    bool                                                error_;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLING_H_
