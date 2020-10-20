#ifndef K_IO_NONBLOCKINGSTREAMINTERFACE_H_
#define K_IO_NONBLOCKINGSTREAMINTERFACE_H_

#include <K/IO/StreamInterface.h>

namespace K {
//! I/O.
namespace IO {

//! Interface to entities providing nonblocking stream I/O.
class NonBlockingStreamInterface : public virtual StreamInterface {
  public:
    class HandlerInterface : public virtual K::Core::Interface {
        virtual void OnReadyRead() = 0;
        virtual void OnReadyWrite() = 0;
    };

    //! Registers the specified handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler. In this case, when the method returns, it
     *  is guaranteed that the handler will not be called again.
     */
    virtual void Register(HandlerInterface *handler) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_NONBLOCKINGSTREAMINTERFACE_H_
