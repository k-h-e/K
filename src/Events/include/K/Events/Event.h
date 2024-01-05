/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_EVENT_H_
#define K_EVENTS_EVENT_H_

#include <cstddef>
#include <K/Core/SerializableInterface.h>

namespace K {
//! Event mechanism.
namespace Events {

//! Base class to events.
/*!
 *  Events exclusively contain "data" fields and never directly reference any other objects. If the data describes
 *  aspects of an actor, the actor is identified via a non-negative integer actor id.
 */
class Event : public virtual Core::SerializableInterface {
  public:
    //! Threadsafe.
    class EventType {
      public:
        //! Pass a verbose name used to initially compute a hash value. Only pass C-style string literals (must live
        //! forever and be safely accessible by multiple threads).
        EventType(const char *aName);
            // Copyable, movable.
        const char   *name;
        const size_t id;
    };
    
    //! Returns the type of the event.
    virtual const EventType &Type() const = 0;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENT_H_
