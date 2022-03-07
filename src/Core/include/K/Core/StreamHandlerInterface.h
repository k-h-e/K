#ifndef K_CORE_STREAMHANDLERINTERFACE_H_
#define K_CORE_STREAMHANDLERINTERFACE_H_

#include <K/Core/DataHandlerInterface.h>

namespace K {
namespace Core {

//! Interface to entities accepting and processing stream data.
class StreamHandlerInterface : public virtual DataHandlerInterface {
  public:
    //! Hands over another portion of stream data to the handler.
    virtual void HandleStreamData(const void *data, int dataSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_STREAMHANDLERINTERFACE_H_

