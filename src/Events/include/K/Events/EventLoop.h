#ifndef K_EVENTS_EVENTLOOP_H_
#define K_EVENTS_EVENTLOOP_H_

#include <cassert>
#include <vector>
#include <memory>
#include <unordered_map>
#include <K/Core/Log.h>
#include <K/Core/Buffer.h>
#include <K/Events/Event.h>
#include <K/Events/EventReceiverInterface.h>
#include <K/Events/EventLoopHub.h>

namespace K {
namespace Events {

//! Event loop.
/*!
 *  The \ref EventLoop s of multiple threads are integrated using an \ref EventLoopHub.
 *
 *  <c>EventClass</c> must derive from <c>Event</c> and implement a method
 *  <c>void Dispatch(EventHandlerClass *handler) const</c>.
 *
 *  \ingroup Events
 */
template<class EventClass, class EventHandlerClass>
class EventLoop : public virtual EventReceiverInterface {
  public:
    EventLoop(std::shared_ptr<EventLoopHub> hub);
    EventLoop(const EventLoop &other)            = delete;
    EventLoop &operator=(const EventLoop &other) = delete;
    EventLoop(EventLoop &&other)                 = delete;
    EventLoop &operator=(EventLoop &&other)      = delete;

    //! Registers the specified event.
    void RegisterEvent(std::unique_ptr<EventClass> protoType);
    //! Registers a handler for the specified event, as a weak reference.
    /*!
     *  A given handler must be registered for a given event only once.
     */
    void RegisterHandler(const Event::EventType &eventType, EventHandlerClass *handler);
    //! Unregisters all event handlers.
    void UnregisterHandlers();
    //! Runs the event loop until shutdown is requested (via the hub).
    void Run();
    //! Runs the event loop until the next event of the specified type has been dispatched.
    /*!
     *  Pass <c>nullptr</c> to run until shutdown is requested (via the hub).
     *
     *  \return <c>false</c> in case shutdown has been requested.
     */
    bool RunUntilEventOfType(const Event::EventType *eventType);
    //! Runs a batch of events, including all the events posted up until now.
    /*!
     *  \return <c>false</c> in case shutdown has been requested.
     */
    bool RunBatch();
    //! Posts the specified event for execution on the loop.
    /*!
     *  This is the only method that may be called while any \ref Run() method executes, that is: from event handlers
     *  invoked by the loop.
     */
    virtual void Post(const Event &event);
        
  private:
    struct EventInfo {
        std::unique_ptr<EventClass>      prototype;
        std::vector<EventHandlerClass *> handlers;
        EventInfo(std::unique_ptr<EventClass> proto) : prototype(std::move(proto)) {}
    };
    
    EventClass *DispatchOne();

    std::vector<EventInfo>          events_;
    std::unordered_map<size_t, int> idToSlotMap_;
    std::unique_ptr<Core::Buffer>   eventsToDispatch_;
    Core::Buffer::Reader            reader_;
    std::shared_ptr<EventLoopHub>   hub_;
    int                             hubClientId_;
    bool                            running_;
};

template<class EventClass, class EventHandlerClass>
EventLoop<EventClass, EventHandlerClass>::EventLoop(std::shared_ptr<EventLoopHub> hub)
    : eventsToDispatch_(new Core::Buffer()),
      reader_(eventsToDispatch_->GetReader()),
      hub_(hub),
      hubClientId_(hub->RegisterEventLoop()),
      running_(false) {
    // Nop.
}

template<class EventClass, class EventHandlerClass>
void EventLoop<EventClass, EventHandlerClass>::RegisterEvent(std::unique_ptr<EventClass> protoType) {
    assert(!running_);
    assert(protoType.get() != nullptr);
    size_t id             = protoType->Type().id;
    bool   alreadyPresent = (idToSlotMap_.find(id) != idToSlotMap_.end());
    if (alreadyPresent) {
        K::Core::Log::Print(K::Core::Log::Level::Warning, this, [&]{
            return std::string("hash collision while registering event \"") + protoType->Type().name + "\""; });
    }
    assert(!alreadyPresent);
    int slot = (int)events_.size();
    events_.push_back(EventInfo(std::move(protoType)));
    idToSlotMap_[id] = slot;
    bool mappingRegistered = hub_->RegisterEventIdToSlotMapping(id, slot);
    assert(mappingRegistered);
}

template<class EventClass, class EventHandlerClass>
void EventLoop<EventClass, EventHandlerClass>::RegisterHandler(const Event::EventType &eventType,
                                                               EventHandlerClass *handlerToRegister) {
    assert(!running_);
    auto infoIter = idToSlotMap_.find(eventType.id);
    assert(infoIter != idToSlotMap_.end());
    for (EventHandlerClass *handler : events_[infoIter->second].handlers) {
        assert(handler != handlerToRegister);
    }
    events_[infoIter->second].handlers.push_back(handlerToRegister);
}

template<class EventClass, class EventHandlerClass>
void EventLoop<EventClass, EventHandlerClass>::UnregisterHandlers() {
    assert(!running_);
    for (EventInfo &info : events_) {
        info.handlers.clear();
    }
}

template<class EventClass, class EventHandlerClass>
void EventLoop<EventClass, EventHandlerClass>::Run() {
    RunUntilEventOfType(nullptr);
}
    
template<class EventClass, class EventHandlerClass>
bool EventLoop<EventClass, EventHandlerClass>::RunUntilEventOfType(const Event::EventType *eventType) {
    assert(!running_);
    running_ = true;
    
    bool done = false;
    while (!done) {
        bool haveEvents = true;
        while (haveEvents) {
            EventClass *event = DispatchOne();
            if (event) {
                if (eventType && (event->Type().id == eventType->id)) {
                    running_ = false;
                    return true;
                }
            }
            else {
                haveEvents = false;
            }
        }
        
        if (!hub_->GetEvents(hubClientId_, &eventsToDispatch_, false)) {
            done = true;
        }
        reader_ = eventsToDispatch_->GetReader();
    }

    running_ = false;
    return false;
}

template<class EventClass, class EventHandlerClass>
bool EventLoop<EventClass, EventHandlerClass>::RunBatch() {
    assert(!running_);
    running_ = true;

    bool done          = false;
    bool fetchedEvents = false;
    while (!done) {
        while (DispatchOne()) {
            // Nop.
        }

        if (fetchedEvents) {
            running_ = false;
            return true;
        }

        done = !hub_->GetEvents(hubClientId_, &eventsToDispatch_, true);
        reader_ = eventsToDispatch_->GetReader();
        fetchedEvents = true;
    }

    running_ = false;
    return false;
}

template<class EventClass, class EventHandlerClass>
void EventLoop<EventClass, EventHandlerClass>::Post(const Event &event) {
    hub_->Post(event);
}

template<class EventClass, class EventHandlerClass>
EventClass *EventLoop<EventClass, EventHandlerClass>::DispatchOne() {
    int slot;
    if (reader_.ReadBlock(&slot, sizeof(slot))) {
        EventInfo  &info  = events_[slot];
        EventClass &event = *(info.prototype);
        event.Deserialize(&reader_);
        for (EventHandlerClass *handler : info.handlers) {
            event.Dispatch(handler);
        }
        return &event;
    }
    else {
        return nullptr;
    }
}

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENTLOOP_H_
