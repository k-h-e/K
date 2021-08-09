#ifndef K_IO_DATAHANDLERINTERFACE_H_
#define K_IO_DATAHANDLERINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace IO {

//! Interface to entities accepting and processing data.
class DataHandlerInterface : public virtual K::Core::Interface {
  public:
    //! Gets called once to indicate that there will be no more data because the source has reached EOF.
    virtual void HandleEof() = 0;
    //! Gets called once to indicate that there will be no more data because the source has entered error state (see
    //! <c>K::Core::ErrorInterface</c>).
    virtual void HandleError() = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DATAHANDLERINTERFACE_H_

