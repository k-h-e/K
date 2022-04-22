#ifndef K_EVENTS_EVENTBUSINTERFACE_H_
#define K_EVENTS_EVENTBUSINTERFACE_H_

#include <memory>
#include <K/Events/Event.h>
#include <K/Events/EventReceiverInterface.h>

namespace K {
namespace Events {

//! Interface to event bus variants.
template<class EventClass, class EventHandlerClass>
class EventBusInterface : public virtual EventReceiverInterface {
  public:
    //! Registers the specified event.
    virtual void RegisterEvent(std::unique_ptr<EventClass> protoType) = 0;
    //! Registers a handler for the specified event, as a weak reference.
    /*!
     *  A given handler must be registered for a given event only once.
     *
     *  May get called from event handlers invoked by the bus.
     */
    virtual void RegisterHandler(const Event::EventType &eventType, EventHandlerClass *handler) = 0;
    //! Unregisters the specified event handler.
    /*!
     *  When this method returns, it is guaranteed that the bus will not call the handler again.
     *
     *  May get called from event handlers invoked by the bus.
     */
    virtual void UnregisterHandler(EventHandlerClass *handler) = 0;

    //! Posts the specified event to the receiver.
    /*!
     *  May get called from event handlers invoked by the bus.
     */
    void Post(const Event &event) override = 0;
};

}    // Namespace Events.
}    // Namespace K.


#endif    // K_EVENTS_EVENTBUSINTERFACE_H_
