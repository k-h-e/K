/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_EVENTFILTERCONFIGURATION_H_
#define K_EVENTS_EVENTFILTERCONFIGURATION_H_

#include <vector>

namespace K {
namespace Events {

//! Holds configuration information for event filtering.
class EventFilterConfiguration {
  public:
    //! Sets whether the event type given by the specified type slot is to be filtered out or not.
    void SetFilteredOut(int eventTypeSlot, bool filteredOut);
    //! Tells whether the event type given by the specified type slot is to be filtered out or not.
    bool FilteredOut(int eventTypeSlot) const;

  private:
    void Grow(int eventTypeSlot);

    std::vector<bool> filterOutFlags_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENTFILTERCONFIGURATION_H_
