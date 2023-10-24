/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_ERRORINTERFACE_H_
#define K_CORE_ERRORINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to entities that can enter error state as part of their error handling strategy.
class ErrorStateInterface : public virtual Interface {
  public:
    //! Tells whether or not the object has entered error state.
    /*!
     *  When in error state, all operations on the object have no effect and indicate failure (for operations returning
     *  results). However, as an exception, in case the object allows for the registration of listeners/handlers,
     *  unregistering of listeners/handlers must still work in error state, and the associated guarantees must still be
     *  enforced.
     *
     *  Error state is persistent insofar as once the object has entered error state, it does not automatically leave it
     *  again.
     */
    virtual bool ErrorState() const = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ERRORINTERFACE_H_
