#ifndef K_IO_ASYNCREADINTERFACE_H_
#define K_IO_ASYNCREADINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace IO {

//! Interface to entities providing asynchronous stream reading.
class AsyncReadInterface : public virtual K::Core::Interface {
  public:
    class HandlerInterface : public virtual K::Core::Interface {
      public:
        //! Gets called to asynchronously deliver read data.
        virtual void OnDataRead(void *data, int dataSize) = 0;
        //! Gets called once when reading reaches EOF.
        virtual void OnEof() = 0;
        //! Gets called once when the stream enters error state (see K::Core::ErrorInterface).
        virtual void OnError() = 0;
    };

    //! Registers the specified asynchronous read handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered read handler. In this case, when the method returns,
     *  it is guaranteed that the handler will not be called again.
     *
     *  The handler methods will get called on an arbitrary thread.
     */
    virtual void Register(HandlerInterface *handler) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_ASYNCREADINTERFACE_H_
