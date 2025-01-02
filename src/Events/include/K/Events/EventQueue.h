/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_EVENTQUEUE_H_
#define K_EVENTS_EVENTQUEUE_H_

#include <K/Core/Log.h>
#include <K/Core/RunLoop.h>
#include <K/Events/EventLoop.h>
#include <K/Events/EventNotifier.h>

namespace K {
namespace Events {

//! <c>RunLoop</c>-enabled analogon to <c>EventLoop</c>.
template<class EventClass, class EventHandlerClass>
class EventQueue : public virtual Events::EventBusInterface<EventClass, EventHandlerClass>,
                   private virtual EventNotifier::HandlerInterface,
                   private virtual Core::RunLoop::ClientInterface {
  public:
    EventQueue(const std::shared_ptr<EventHub> &hub, const std::shared_ptr<Core::RunLoop> &runLoop);
    EventQueue()                                   = delete;
    EventQueue(const EventQueue &other)            = delete;
    EventQueue &operator=(const EventQueue &other) = delete;
    EventQueue(EventQueue &&other)                 = delete;
    EventQueue &operator=(EventQueue &&other)      = delete;
    ~EventQueue();

    void RegisterEvent(std::unique_ptr<EventClass> protoType) override;
    void RegisterHandler(const Event::EventType &eventType, EventHandlerClass *handler) override;
    void UnregisterHandler(EventHandlerClass *handler) override;
    void Post(const Event &event) override;

  private:
    void Dispatch();

    // EventNotifier::HandlerInterface...
    void OnEventsAvailable() override;

    // RunLoop::ClientInterface...
    void Activate(bool deepActivation) override;

    const std::shared_ptr<K::Core::RunLoop>  runLoop_;
    int                                      runLoopClientId_;
    std::unique_ptr<EventNotifier>           eventNotifier_;
    EventLoop<EventClass, EventHandlerClass> eventLoop_;
    bool                                     dispatching_;
    bool                                     activationRequested_;
};

template<class EventClass, class EventHandlerClass>
EventQueue<EventClass, EventHandlerClass>::EventQueue(const std::shared_ptr<EventHub> &hub,
                                                      const std::shared_ptr<K::Core::RunLoop> &runLoop)
        : runLoop_{runLoop},
          runLoopClientId_ { runLoop->AddClient(this) },
          eventLoop_{hub},
          dispatching_{false},
          activationRequested_{false}  {
    eventNotifier_ = std::make_unique<EventNotifier>(hub, eventLoop_.hubClientId(), runLoop_);
    eventNotifier_->Register(this);
    // Nop.
}

template<class EventClass, class EventHandlerClass>
EventQueue<EventClass, EventHandlerClass>::~EventQueue() {
    runLoop_->RemoveClient(runLoopClientId_);
    
    eventNotifier_->Register(nullptr);    // Defensive.
    eventNotifier_.reset();
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::RegisterEvent(std::unique_ptr<EventClass> protoType) {
    eventLoop_.RegisterEvent(std::move(protoType));
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::RegisterHandler(const Event::EventType &eventType,
                                                                EventHandlerClass *handler) {
    eventLoop_.RegisterHandler(eventType, handler);
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::UnregisterHandler(EventHandlerClass *handler) {
    eventLoop_.UnregisterHandler(handler);
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::Post(const Event &event) {
    eventLoop_.Post(event);

    if (!dispatching_) {
        if (!activationRequested_) {
            runLoop_->RequestActivation(runLoopClientId_, false);
            activationRequested_ = true;
        }
    }
}

// ---

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::Dispatch() {
    dispatching_ = true;
    bool shutDownRequested = !eventLoop_.Dispatch(true);
    dispatching_ = false;

    if (shutDownRequested) {
        K::Core::Log::Print(K::Core::Log::Level::Debug, this, [&]{
            return "event hub signalled termination, requesting run loop termination";
        });
        runLoop_->RequestTermination();
    }
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::OnEventsAvailable() {
    Dispatch();
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::Activate(bool deepActivation) {
    (void) deepActivation;
    activationRequested_ = false;
    Dispatch();
}

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENTQUEUE_H_
