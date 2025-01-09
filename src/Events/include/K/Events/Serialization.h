/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_SERIALIZATION_H_
#define K_EVENTS_SERIALIZATION_H_

#include <cassert>
#include <vector>

#include <K/Core/BlockingInStreamInterface.h>
#include <K/Events/Event.h>
#include <K/Events/EventInfo.h>

namespace K {
    namespace Core {
        class SeekableBlockingOutStreamInterface;
    }
    namespace Events {
        class Event;
    }
}

namespace K {
namespace Events {

//! Serializes the specified event for event bus transmission.
void Serialize(const Event &event, int typeSlot, Core::SeekableBlockingOutStreamInterface &stream);
//! Deserializes the next event from the specified event bus transmission data stream.
template<typename EventClass, typename EventHandlerClass>
EventInfo<EventClass, EventHandlerClass> *DeserializeIfNotEof(
    Core::BlockingInStreamInterface &stream, const std::vector<EventInfo<EventClass, EventHandlerClass>> &events);
//! Deserializes the next event header from the specified event bus transmission data stream.
void DeserializeHeader(Core::BlockingInStreamInterface &stream, int &outTypeSlot, int &outSize, bool &outEofDetected);

template<typename EventClass, typename EventHandlerClass>
EventInfo<EventClass, EventHandlerClass> *DeserializeIfNotEof(
        Core::BlockingInStreamInterface &stream, std::vector<EventInfo<EventClass, EventHandlerClass>> &events) {
    int  typeSlot;
    int  size;
    bool eofDetected;
    DeserializeHeader(stream, typeSlot, size, eofDetected);
    if (eofDetected) {
        return nullptr;
    }

    assert(!stream.ErrorState());
    assert((typeSlot >= 0) && (typeSlot < static_cast<int>(events.size())));
    // Ignoring size.

    EventInfo<EventClass, EventHandlerClass> &info  { events[typeSlot] };
    Event                                    &event { *(info.protoType) };
    event.Deserialize(stream);
    return &info;
}

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_SERIALIZATION_H_
