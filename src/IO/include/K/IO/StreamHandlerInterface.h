#ifndef K_IO_STREAMHANDLERINTERFACE_H_
#define K_IO_STREAMHANDLERINTERFACE_H_

#include <K/IO/DataHandlerInterface.h>

namespace K {
namespace IO {

//! Interface to entities accepting and processing stream data.
class StreamHandlerInterface : public virtual DataHandlerInterface {
  public:
    //! Hands over another portion of stream data to the handler.
    virtual void HandleStreamData(const void *data, int dataSize) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMHANDLERINTERFACE_H_

