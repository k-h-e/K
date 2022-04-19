#ifndef K_EVENTS_FRAMEWORK_EVENTNOTIFIER_H_
#define K_EVENTS_FRAMEWORK_EVENTNOTIFIER_H_

#include <memory>
#include <K/Core/Interface.h>

namespace K {
    namespace Core {
        namespace Framework {
            class RunLoop;
        }
    }
    namespace Events {
        class EventHub;
    }
}

namespace K {
namespace Events {
namespace Framework {

//! <c>Core::Framework::RunLoop</c>-enabled client to a <c>EventLoopHub</c>, notifying a handler on the
//! thread associated with the run loop when events become available for processing by the respective hub client (event
//! loop).
/*!
 *  The respective hub client (event loop) is expected to remain registered with the hub as long as the notifier lives.
 *  For as long as it lives, the notifier registers itself with the hub as handler for the respective hub client (event
 *  loop).
 *
 *  The class is intended to be used in the implementation of NetworkEventCoupling and others.
 */
class EventNotifier : public virtual Core::Interface {
  public:
    //! Interface to handlers that clients can register with an event notifier.
    class HandlerInterface : public virtual Core::Interface {
      public:
        //! Indicates that events have become available in the <c>EventLoopHub</c> for the respective client (event
        //! loop).
        /*!
         *  The notifier client needs to fetch all its events before it will get notified again.
         *
         *  \param id
         *  ID that was given when the handler was registered.
         */
        virtual void OnEventsAvailable(int id) = 0;
    };

    EventNotifier(const std::shared_ptr<EventHub> &hub, int clientLoopId,
                  const std::shared_ptr<Core::Framework::RunLoop> &runLoop);
    EventNotifier(const EventNotifier &other)            = delete;
    EventNotifier &operator=(const EventNotifier &other) = delete;
    EventNotifier(EventNotifier &&other)                 = delete;
    EventNotifier &operator=(EventNotifier &&other)      = delete;
    ~EventNotifier();

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the event
     *  notifier.
     *
     *  The handler is expected to outlive the event notifier.
     *
     *  \param id
     *  ID to be passed along with handler activations by the event notifier. Useful in case one wants to use one
     *  handler with multiple event notifiers.
     */
    void Register(HandlerInterface *handler, int id);

  private:
    struct LoopThreadState;
    class  Adapter;

    std::unique_ptr<LoopThreadState> loopThreadState_;
    std::unique_ptr<Adapter>         adapter_;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_EVENTNOTIFIER_H_
