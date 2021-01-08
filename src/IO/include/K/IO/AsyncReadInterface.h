#ifndef K_IO_ASYNCREADINTERFACE_H_
#define K_IO_ASYNCREADINTERFACE_H_

#include <memory>
#include <K/Core/Interface.h>

namespace K {
namespace IO {

//! Interface to entities providing asynchronous stream reading.
class AsyncReadInterface : public virtual K::Core::Interface {
  public:
    class HandlerInterface : public virtual K::Core::Interface {
      public:
        //! Gets called to asynchronously deliver read data.
        virtual void OnDataRead(const void *data, int dataSize) = 0;
        //! Gets called once when reading reaches EOF.
        virtual void OnEof() = 0;
        //! Gets called once when the stream enters error state (see K::Core::ErrorInterface).
        virtual void OnError() = 0;
    };

    //! Registers the specified asynchronous read handler.
    /*!
     *  The handler methods will get called on an arbitrary thread and must not call back into the stream.
     */
    virtual bool Register(const std::shared_ptr<HandlerInterface> &handler) = 0;
    //! Unregisters the specified asynchronous read handler if it was registered.
    /*!
     *  When the method returns, it is guaranteed that the handler will not be called again. If registering another read
     *  handler later, read data might have been missed - with the exception of the initial time after stream
     *  construction, there is no garantee that the stream buffers read data while no read handler is registered.
     */
    virtual void Unregister(const std::shared_ptr<HandlerInterface> &handler) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_ASYNCREADINTERFACE_H_
