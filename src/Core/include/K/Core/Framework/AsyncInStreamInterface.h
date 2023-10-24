/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_FRAMEWORK_ASYNCREADINTERFACE_H_
#define K_CORE_FRAMEWORK_ASYNCREADINTERFACE_H_

#include <K/Core/InStreamInterface.h>

namespace K {
    namespace Core {
        class StreamHandlerInterface;
    }
}

namespace K {
namespace Core {
namespace Framework {

//! Interface to framework input streams providing asynchronous reading.
class AsyncInStreamInterface : public virtual Core::InStreamInterface {
  public:
    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the stream.
     *
     *  The handler is expected to outlive the stream. It will not get called upon stream destruction.
     *
     *  \param activationId
     *  ID to be passed along with handler activations for the stream. Useful in case one wants to use one handler
     *  with multiple streams.
     */
    virtual void Register(Core::StreamHandlerInterface *handler, int activationId) = 0;
};

}    // Namespace Framework.
}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_FRAMEWORK_ASYNCREADINTERFACE_H_
