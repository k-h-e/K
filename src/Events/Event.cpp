/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Events/Event.h>

#include <functional>

namespace K {
namespace Events {

Event::EventType::EventType(const char *aName)
        : name(aName),
          id(std::hash<const char *>()(aName)) {
}

}    // Namespace Events.
}    // Namespace K.
