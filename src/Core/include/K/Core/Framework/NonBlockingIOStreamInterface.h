////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_FRAMEWORK_NONBLOCKINGSTREAMINTERFACE_H_
#define K_CORE_FRAMEWORK_NONBLOCKINGSTREAMINTERFACE_H_

#include <memory>
#include <K/Core/NonBlockingIOStreamInterface.h>

namespace K {
namespace Core {
namespace Framework {

//! Interface to framework streams providing nonblocking stream I/O.
class NonBlockingIOStreamInterface : public virtual Core::NonBlockingIOStreamInterface {
  public:
    //! Interface to handlers for non-blocking framework streams.
    class HandlerInterface : public virtual K::Core::Interface {
      public:
        //! Tells the handler that the stream has just become ready to be read from.
        /*!
         *  \param id
         *  ID that was given when the handler was registered.
         */
        virtual void OnStreamReadyRead(int id) = 0;
        //! Tells the handler that the stream has just become ready to be written to.
        /*!
         *  \param id
         *  ID that was given when the handler was registered.
         */
        virtual void OnStreamReadyWrite(int id) = 0;
    };

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler.
     *
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the stream.
     *
     *  The handler is expected to outlive the stream.
     *
     *  \param id
     *  ID to be passed along with handler activations for the stream. Useful in case one wants to use one handler
     *  with multiple streams.
     */
    virtual void Register(HandlerInterface *handler, int id) = 0;
};

}    // Namespace Framework.
}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_FRAMEWORK_NONBLOCKINGSTREAMINTERFACE_H_
