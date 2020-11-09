#ifndef K_IO_BUFFEREDCONNECTIONSTREAMINTERFACE_H_
#define K_IO_BUFFEREDCONNECTIONSTREAMINTERFACE_H_

#include <K/IO/NonBlockingStreamInterface.h>

namespace K {
//! I/O.
namespace IO {

//! Interface to buffered connection streams.
class BufferedConnectionStreamInterface : public virtual NonBlockingStreamInterface {
  public:
    class ReadHandlerInterface : public virtual K::Core::Interface {
      public:
        //! Gets called to asynchronously deliver read data.
        virtual void OnDataRead(void *data, int dataSize) = 0;
    };

    //! Registers the specified read handler.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered read handler. In this case, when the method returns,
     *  it is guaranteed that the handler will not be called again.
     *
     *  The handler methods will get called on an arbitrary thread.
     *
     *  Registering a dedicated read handler will turn off data to be fed to the regular read buffer.
     */
    virtual void Register(ReadHandlerInterface *handler) = 0;
    //! Writes a binary item of specified size (in bytes).
    /*!
     *  \return
     *  <c>false</c> in case of failure. Error state will then be raised on the stream. Note that failure cases include
     *  write buffer overflow.
     */
    virtual bool WriteItem(const void *item, int itemSize) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFEREDCONNECTIONSTREAMINTERFACE_H_
