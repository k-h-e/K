/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_PROGRESSTRACKER_H_
#define K_CORE_PROGRESSTRACKER_H_

#include <K/Core/ProgressTrackerInterface.h>

namespace K {
namespace Core {

//! Tracks progress of an activity that has a given number of discrete steps (of comparable complexity).
class ProgressTracker : public virtual ProgressTrackerInterface {
  public:
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_PROGRESSTRACKER_H_
