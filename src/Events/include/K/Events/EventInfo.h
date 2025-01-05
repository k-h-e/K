/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_EVENTINFO_H_
#define K_EVENTS_EVENTINFO_H_

#include <memory>

namespace K {
namespace Events {

template<typename EventClass, typename EventHandlerClass>
struct EventInfo {
    std::unique_ptr<EventClass>      protoType;
    std::vector<EventHandlerClass *> handlers;

    EventInfo(std::unique_ptr<EventClass> protoType) : protoType{std::move(protoType)} {}
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENTINFO_H_
