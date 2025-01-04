/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Events/EventFilterConfiguration.h>

#include <cassert>

using std::vector;

namespace K {
namespace Events {

void EventFilterConfiguration::SetFilteredOut(int eventTypeSlot, bool filteredOut) {
    assert(eventTypeSlot >= 0);
    Grow(eventTypeSlot);
    filterOutFlags_[eventTypeSlot] = filteredOut;
}

bool EventFilterConfiguration::FilteredOut(int eventTypeSlot) const {
    assert(eventTypeSlot >= 0);
    if (eventTypeSlot < static_cast<int>(filterOutFlags_.size())) {
        return filterOutFlags_[eventTypeSlot];
    }
    
    return false;
}

// ---

void EventFilterConfiguration::Grow(int eventTypeSlot) {
    if (eventTypeSlot >= static_cast<int>(filterOutFlags_.size())) {
        filterOutFlags_.resize(static_cast<vector<bool>::size_type>(eventTypeSlot + 1), false);
    } 
}

}    // Namespace Events.
}    // Namespace K.
