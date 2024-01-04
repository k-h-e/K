/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_COMPLETIONHANDLERINTERFACE_H_
#define K_CORE_COMPLETIONHANDLERINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to completion handlers.
class CompletionHandlerInterface : public virtual Interface {
  public:
    //! Called upon completion of the specified operation.
    virtual void OnCompletion(int completionId) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_COMPLETIONHANDLERINTERFACE_H_
