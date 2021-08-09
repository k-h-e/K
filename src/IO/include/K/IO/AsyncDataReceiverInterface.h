#ifndef K_IO_ASYNCDATARECEIVERINTERFACE_H_
#define K_IO_ASYNCDATARECEIVERINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace IO {

//! Interface to entities asyncronously accepting and processing data.
class AsyncDataReceiverInterface : public virtual K::Core::Interface {
  public:
    //! Gets called once to indicate that there will be no more data because the source has reached EOF.
    virtual void OnEof() = 0;
    //! Gets called once to indicate that there will be no more data because the source has entered error state (see
    //! <c>K::Core::ErrorInterface</c>).
    virtual void OnError() = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_ASYNCDATARECEIVERINTERFACE_H_

