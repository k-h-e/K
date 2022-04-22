////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Events/Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_EVENTS_FRAMEWORK_EVENTQUEUE_H_
#define K_EVENTS_FRAMEWORK_EVENTQUEUE_H_

#include <K/Core/Framework/RunLoop.h>
#include <K/Events/EventLoop.h>
#include <K/Events/Framework/EventNotifier.h>

namespace K {
    namespace Core {
        namespace Framework {
            class RunLoop;
        }
    }
}

namespace K {
namespace Events {
namespace Framework {

//! Framework-enabled analogon to <c>EventLoop</c>.
template<class EventClass, class EventHandlerClass>
class EventQueue : public virtual Events::EventBusInterface<EventClass, EventHandlerClass> {
  public:
    EventQueue(const std::shared_ptr<EventHub> &hub, const std::shared_ptr<Core::Framework::RunLoop> &runLoop);
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
    class Core : public virtual EventNotifier::HandlerInterface {
      public:
        Core(const std::shared_ptr<EventHub> &hub, const std::shared_ptr<K::Core::Framework::RunLoop> &runLoop);
        Core()                                   = delete;
        Core(const Core &other)            = delete;
        Core &operator=(const Core &other) = delete;
        Core(Core &&other)                 = delete;
        Core &operator=(Core &&other)      = delete;
        ~Core();

        void RegisterEvent(std::unique_ptr<EventClass> protoType);
        void RegisterHandler(const Event::EventType &eventType, EventHandlerClass *handler);
        void UnregisterHandler(EventHandlerClass *handler);
        void Post(const Event &event);
        void OnEventsAvailable(int id) override;

      private:
        const std::shared_ptr<K::Core::Framework::RunLoop> runLoop_;
        std::unique_ptr<EventNotifier>                     eventNotifier_;
        EventLoop<EventClass, EventHandlerClass>           eventLoop_;
    };

    std::unique_ptr<Core> core_;
};

template<class EventClass, class EventHandlerClass>
EventQueue<EventClass, EventHandlerClass>::EventQueue(const std::shared_ptr<EventHub> &hub,
                                                      const std::shared_ptr<K::Core::Framework::RunLoop> &runLoop)
        : core_(std::make_unique<Core>(hub, runLoop)) {
    // Nop.
}

template<class EventClass, class EventHandlerClass>
EventQueue<EventClass, EventHandlerClass>::~EventQueue() {
    // Nop.
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::RegisterEvent(std::unique_ptr<EventClass> protoType) {
    core_->RegisterEvent(std::move(protoType));
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::RegisterHandler(const Event::EventType &eventType,
                                                                EventHandlerClass *handler) {
    core_->RegisterHandler(eventType, handler);
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::UnregisterHandler(EventHandlerClass *handler) {
    core_->UnregisterHandler(handler);
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::Post(const Event &event) {
    core_->Post(event);
}

template<class EventClass, class EventHandlerClass>
EventQueue<EventClass, EventHandlerClass>::Core::Core(const std::shared_ptr<EventHub> &hub,
                                                      const std::shared_ptr<K::Core::Framework::RunLoop> &runLoop)
        : runLoop_(runLoop),
          eventLoop_(hub) {
    eventNotifier_ = std::make_unique<EventNotifier>(hub, eventLoop_.hubClientId(), runLoop_);
    eventNotifier_->Register(this, 0);
}

template<class EventClass, class EventHandlerClass>
EventQueue<EventClass, EventHandlerClass>::Core::~Core() {
    eventNotifier_->Register(nullptr, 0);    // Defensive.
    eventNotifier_.reset();
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::Core::RegisterEvent(std::unique_ptr<EventClass> protoType) {
    eventLoop_.RegisterEvent(std::move(protoType));
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::Core::RegisterHandler(const Event::EventType &eventType,
                                                                      EventHandlerClass *handler) {
    eventLoop_.RegisterHandler(eventType, handler);
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::Core::UnregisterHandler(EventHandlerClass *handler) {
    eventLoop_.UnregisterHandler(handler);
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::Core::Post(const Event &event) {
    eventLoop_.Post(event);
}

template<class EventClass, class EventHandlerClass>
void EventQueue<EventClass, EventHandlerClass>::Core::OnEventsAvailable(int id) {
    (void)id;
    if (!eventLoop_.Dispatch(true)) {
        runLoop_->RequestTermination();
    }
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_EVENTQUEUE_H_
