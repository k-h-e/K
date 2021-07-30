#ifndef K_IO_NONBLOCKINGSTREAMINTERFACE_H_
#define K_IO_NONBLOCKINGSTREAMINTERFACE_H_

#include <memory>
#include <K/IO/GenericReadInterface.h>
#include <K/IO/GenericWriteInterface.h>

namespace K {
namespace IO {

//! Interface to streams providing nonblocking stream I/O.
class NonBlockingStreamInterface : public virtual GenericReadInterface,
                                   public virtual GenericWriteInterface {
  public:
    class HandlerInterface : public virtual K::Core::Interface {
      public:
        //! Gets called when data becomes available to read.
        /*!
         *  Once called, will only get called again when Read() has at least once reported 0 bytes in the meantime.
         */
        virtual void OnReadyRead() = 0;
        //! Gets called when the stream becomes ready to write.
        /*!
         *  Once called, will only get called again when Write() has at least once accepted 0 bytes in the meantime.
         */
        virtual void OnReadyWrite() = 0;
    };

    //! Registers the specified handler.
    /*!
     *  The handler methods will get called on an arbitrary thread and must not call back into the stream.
     */
    virtual bool Register(const std::shared_ptr<HandlerInterface> &handler) = 0;
    //! Unregisters the specified handler if it was registered.
    /*!
     *  When the method returns, it is guaranteed that the handler will not be called again.
     */
    virtual void Unregister(const std::shared_ptr<HandlerInterface> &handler) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_NONBLOCKINGSTREAMINTERFACE_H_
