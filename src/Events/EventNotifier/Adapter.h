/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_EVENTNOTIFIER_ADAPTER_H_
#define K_EVENTS_EVENTNOTIFIER_ADAPTER_H_

#include <K/Events/EventHub.h>
#include <K/Events/EventNotifier.h>

namespace K {
namespace Events {

//! To be registered with the event loop hub.
class EventNotifier::Adapter : public virtual EventHub::HandlerInterface {
  public:
    Adapter(const std::shared_ptr<Core::RunLoop> &runLoop, int runLoopClientId);
    Adapter(const Adapter &other)            = delete;
    Adapter &operator=(const Adapter &other) = delete;
    Adapter(Adapter &&other)                 = delete;
    Adapter &operator=(Adapter &&other)      = delete;
    ~Adapter()                               = default;

    void OnEventsAvailable() override;

  private:
    const std::shared_ptr<Core::RunLoop> runLoop_;
    int                                  runLoopClientId_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENTNOTIFIER_ADAPTER_H_
