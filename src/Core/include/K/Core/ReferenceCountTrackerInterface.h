/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_REFERENCECOUNTTRACKERINTERFACE_H_
#define K_CORE_REFERENCECOUNTTRACKERINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to entities keeping track of the number of references to some given object.
class ReferenceCountTrackerInterface : public virtual Interface {
  public:
    //! Informs the tracker that one reference has just been added.
    virtual void OnReferenceAdded() = 0;
    //! Informs the tracker that one reference has just been removed.
    virtual void OnReferenceRemoved() = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_REFERENCECOUNTTRACKERINTERFACE_H_
