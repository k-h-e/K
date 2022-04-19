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
#include <K/Events/EventHub.h>

namespace K {
namespace Events {

//! Event loop.
/*!
 *  The \ref EventLoop s of multiple threads are integrated using an \ref EventLoopHub.
 *
 *  <c>EventClass</c> must derive from <c>Event</c> and implement a method <c>void Dispatch(EventHandlerClass *handler)
 *  const</c>.
 */
template<class EventClass, class EventHandlerClass>
class EventLoop : public virtual EventReceiverInterface {
  public:
    EventLoop(std::shared_ptr<EventHub> hub);
    EventLoop(const EventLoop &other)            = delete;
    EventLoop &operator=(const EventLoop &other) = delete;
    EventLoop(EventLoop &&other)                 = delete;
    EventLoop &operator=(EventLoop &&other)      = delete;

    //! Registers the specified event.
    void RegisterEvent(std::unique_ptr<EventClass> protoType);
    //! Registers a handler for the specified event, as a weak reference.
    /*!
     *  A given handler must be registered for a given event only once.
     *
     *  May get called from event handlers invoked by the loop.
     */
    void RegisterHandler(const Event::EventType &eventType, EventHandlerClass *handler);
    //! Unregisters the specified event handler.
    /*!
     *  When this method returns, it is guaranteed that the loop will not call the handler again.
     *
     *  May get called from event handlers invoked by the loop.
     */
    void UnregisterHandler(EventHandlerClass *handler);
    //! Runs the event loop until shutdown is requested (via the hub).
    void Run();
    //! Runs the event loop until the next event of the specified type has been dispatched.
    /*!
     *  Pass <c>nullptr</c> to run until shutdown is requested (via the hub).
     *
     *  \return <c>false</c> in case shutdown has been requested.
     */
    bool RunUntilEventOfType(const Event::EventType *eventType);
    //! Syncs with the hub once (without waiting) and dispatches all scheduled events (up to and including those that
    //! were retrieved from the hub).
    /*!
     *  \param doFinalSubmit
     *  If set to <c>true</c>, any accumulated posted events will be submitted to the hub when dispatching events is
     *  finished. Otherwise, they remain buffered.
     *
     *  \return <c>false</c> in case shutdown has been requested.
     */
    bool Dispatch(bool doFinalSubmit);
    //! Posts the specified event for execution on the loop.
    /*!
     *  May get called from event handlers invoked by the loop.
     */
    virtual void Post(const Event &event);
        
  private:
    struct EventInfo {
        std::unique_ptr<EventClass>      prototype;
        std::vector<EventHandlerClass *> handlers;
        EventInfo(std::unique_ptr<EventClass> proto) : prototype(std::move(proto)) {}
    };
    
    EventClass *DispatchOne();

    std::vector<EventInfo>           events_;
    std::unordered_map<size_t, int>  idToSlotMap_;
    std::unique_ptr<Core::Buffer>    postedEvents_;
    std::unique_ptr<Core::Buffer>    eventsToDispatch_;
    Core::Buffer::Reader             reader_;
    std::vector<EventHandlerClass *> pendingHandlers_;
    int                              pendingHandlerCursor_;
    std::shared_ptr<EventHub>        hub_;
    int                              hubClientId_;
    bool                             running_;
};

template<class EventClass, class EventHandlerClass>
EventLoop<EventClass, EventHandlerClass>::EventLoop(std::shared_ptr<EventHub> hub)
    : postedEvents_(new Core::Buffer()),
      eventsToDispatch_(new Core::Buffer()),
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
                                                               EventHandlerClass *handler) {
    auto infoIter = idToSlotMap_.find(eventType.id);
    assert(infoIter != idToSlotMap_.end());
    for (EventHandlerClass *aHandler : events_[infoIter->second].handlers) {
        assert(aHandler != handler);
    }
    events_[infoIter->second].handlers.push_back(handler);
}

template<class EventClass, class EventHandlerClass>
void EventLoop<EventClass, EventHandlerClass>::UnregisterHandler(EventHandlerClass *handler) {
    for (EventInfo &info : events_) {
        std::vector<EventHandlerClass *> oldHandlers(info.handlers);
        info.handlers.clear();
        for (EventHandlerClass *aHandler : oldHandlers) {
            if (aHandler != handler) {
                info.handlers.push_back(aHandler);
            }
        }
    }

    // In case we were called from one of our own event handlers, we must make sure the unregistered handler will no
    // longer be called in the process of the remaining event dispatch.
    if (!pendingHandlers_.empty()) {
        std::vector<EventHandlerClass *> oldPendingHandlers(pendingHandlers_);
        pendingHandlers_.clear();
        for (int i = 0; i < static_cast<int>(oldPendingHandlers.size()); ++i) {
            EventHandlerClass *aHandler = oldPendingHandlers[i];
            if ((i <= pendingHandlerCursor_) || (aHandler != handler)) {
                pendingHandlers_.push_back(aHandler);
            }
        }
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
        
        if (hub_->Sync(hubClientId_, &postedEvents_, std::nullopt)) {
            eventsToDispatch_.swap(postedEvents_);
        } else {
            eventsToDispatch_->Clear();
            done = true;
        }

        reader_ = eventsToDispatch_->GetReader();
        postedEvents_->Clear();
    }

    running_ = false;
    return false;
}

template<class EventClass, class EventHandlerClass>
bool EventLoop<EventClass, EventHandlerClass>::Dispatch(bool doFinalSubmit) {
    assert(!running_);
    running_ = true;

    bool shutDownRequested = false;
    bool didSync           = false;
    while (!shutDownRequested) {
        while (DispatchOne()) {
            // Nop.
        }

        if (didSync) {
            if (doFinalSubmit && postedEvents_->DataSize()) {
                hub_->Submit(hubClientId_, postedEvents_->Data(), postedEvents_->DataSize(), false);
                postedEvents_->Clear();
            }
            running_ = false;
            return true;
        }

        shutDownRequested = !hub_->Sync(hubClientId_, &postedEvents_);
        if (shutDownRequested) {
            eventsToDispatch_->Clear();
        } else {
            eventsToDispatch_.swap(postedEvents_);
        }

        reader_ = eventsToDispatch_->GetReader();
        postedEvents_->Clear();
        didSync = true;
    }

    running_ = false;
    return false;
}

template<class EventClass, class EventHandlerClass>
void EventLoop<EventClass, EventHandlerClass>::Post(const Event &event) {
    auto iter = idToSlotMap_.find(event.Type().id);
    assert(iter != idToSlotMap_.end());
    int slot = iter->second;
    postedEvents_->Append(&slot, sizeof(slot));
    event.Serialize(postedEvents_.get());
}

template<class EventClass, class EventHandlerClass>
EventClass *EventLoop<EventClass, EventHandlerClass>::DispatchOne() {
    int slot;
    reader_.ReadItem(&slot, sizeof(slot));
    if (!reader_.ReadFailed()) {
        EventInfo  &info  = events_[slot];
        EventClass &event = *(info.prototype);
        event.Deserialize(&reader_);

        // We allow handlers to be registered and deregistered from event handlers, so we copy the list of handlers for
        // the current event before beginning to dispatch it. Wenn a handler is unregistered during dispatch, it also
        // gets removed from this copied list of pending handlers.
        pendingHandlers_.clear();
        for (EventHandlerClass *handler : info.handlers) {
            pendingHandlers_.push_back(handler);
        }
        pendingHandlerCursor_ = 0;
        while (pendingHandlerCursor_ < static_cast<int>(pendingHandlers_.size())) {
            EventHandlerClass *handler = pendingHandlers_[pendingHandlerCursor_];
            event.Dispatch(handler);
            ++pendingHandlerCursor_;
        }
        pendingHandlers_.clear();

        return &event;
    }
    else {
        return nullptr;
    }
}

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENTLOOP_H_
