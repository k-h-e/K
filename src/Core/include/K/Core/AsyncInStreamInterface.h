/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_ASYNCREADINTERFACE_H_
#define K_CORE_ASYNCREADINTERFACE_H_

#include <K/Core/InStreamInterface.h>

namespace K {
    namespace Core {
        class RawStreamHandlerInterface;
    }
}

namespace K {
namespace Core {

//! Interface to <c>RunLoop</c>-enabled input streams providing asynchronous reading.
class AsyncInStreamInterface : public virtual Core::InStreamInterface {
  public:
    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the stream.
     *
     *  The handler is expected to outlive the stream. It will not get called upon stream destruction.
     */
    virtual void Register(Core::RawStreamHandlerInterface *handler) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ASYNCREADINTERFACE_H_
