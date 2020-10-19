#ifndef K_IO_NONBLOCKINGSTREAMINTERFACE_H_
#define K_IO_NONBLOCKINGSTREAMINTERFACE_H_

#include <K/IO/StreamInterface.h>

namespace K {
//! I/O.
namespace IO {

//! Interface to entities providing nonblocking stream I/O.
class NonBlockingStreamInterface : public virtual StreamInterface {
  public:
    class Listener : public virtual K::Core::Interface {
        virtual void OnReadyRead() = 0;
        virtual void OnReadyWrite() = 0;
    };

    //! Registers the specified listener.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered listener. In this case, when the method returns, it
     *  is guaranteed that the listener will not be called again.
     */
    virtual void RegisterListener(Listener *listener) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_NONBLOCKINGSTREAMINTERFACE_H_
