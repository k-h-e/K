#ifndef K_EVENTS_FRAMEWORK_EVENTNOTIFIER_ADAPTER_H_
#define K_EVENTS_FRAMEWORK_EVENTNOTIFIER_ADAPTER_H_

#include <K/Events/EventHub.h>
#include <K/Events/Framework/EventNotifier.h>

namespace K {
namespace Events {
namespace Framework {

//! To be registered with the event loop hub.
class EventNotifier::Adapter : public virtual EventHub::HandlerInterface {
  public:
    Adapter(const std::shared_ptr<Core::Framework::RunLoop> &runLoop, int runLoopClientId);
    Adapter(const Adapter &other)            = delete;
    Adapter &operator=(const Adapter &other) = delete;
    Adapter(Adapter &&other)                 = delete;
    Adapter &operator=(Adapter &&other)      = delete;
    ~Adapter()                               = default;

    void OnEventsAvailable() override;

  private:
    const std::shared_ptr<Core::Framework::RunLoop> runLoop_;
    int                                             runLoopClientId_;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_EVENTNOTIFIER_ADAPTER_H_
