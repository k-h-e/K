/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_COMPOUNDPROGRESSTRACKER_H_
#define K_CORE_COMPOUNDPROGRESSTRACKER_H_

#include <K/Core/ProgressTrackerInterface.h>

namespace K {
namespace Core {

//! Tracks progress of a compound activity, where the subactivities are tracked by child progress trackers.
class CompoundProgressTracker : public virtual ProgressTrackerInterface {
  public:
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_COMPOUNDPROGRESSTRACKER_H_