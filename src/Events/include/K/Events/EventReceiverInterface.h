/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_EVENTRECEIVERINTERFACE_H_
#define K_EVENTS_EVENTRECEIVERINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Events {

class Event;

//! Interface to event receivers.
class EventReceiverInterface : public virtual Core::Interface {
  public:
    //! Posts the specified event to the receiver.
    virtual void Post(const Event &event) = 0;
};

}    // Namespace Events.
}    // Namespace K.


#endif    // K_EVENTS_EVENTRECEIVERINTERFACE_H_
