/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_COMPOUNDPROGRESSTRACKERINTERFACE_H_
#define K_CORE_COMPOUNDPROGRESSTRACKERINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to compound progress trackers.
class CompoundProgressTrackerInterface : public virtual Interface {
  public:
    //! Registers a new subactivity with the compound progress tracker, and returns an ID for it.
    virtual int RegisterSubActivity() = 0;
    //! Informs the compound progress tracker that the specified subactivity has made progress.
    virtual void OnSubActivityProgress(int activity, int percent) = 0;
    //! Unregisters the specified subactivity from the compound progress tracker.
    virtual void UnregisterSubActivity(int activity) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_COMPOUNDPROGRESSTRACKERINTERFACE_H_