#ifndef K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLING_H_
#define K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLING_H_

#include <memory>
#include <K/Core/ErrorStateInterface.h>

namespace K {
    namespace Core {
        class Timers;
        namespace Framework {
            class RunLoop;
            class Timer;
        }
    }
    namespace IO {
        class KeepAliveParameters;
        namespace Framework {
            class TcpConnection;
        }
    }
    namespace Events {
        class EventHub;
    }
}

namespace K {
namespace Events {
namespace Framework {

//! Extends the event mechanism to other nodes across the network.
class NetworkEventCoupling : public virtual K::Core::ErrorStateInterface {
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
        std::unique_ptr<IO::Framework::TcpConnection> tcpConnection, const std::string &protocolVersion,
        const IO::KeepAliveParameters &keepAliveParameters, const std::shared_ptr<EventHub> &hub,
        const std::shared_ptr<K::Core::Framework::RunLoop> &runLoop, const std::shared_ptr<K::Core::Timers> &timers);
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
    class Core;

    std::unique_ptr<Core> core_;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLING_H_
