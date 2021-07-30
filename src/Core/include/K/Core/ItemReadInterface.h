#ifndef K_CORE_ITEMREADINTERFACE_H_
#define K_CORE_ITEMREADINTERFACE_H_

#include <K/Core/ReadableStreamInterface.h>

namespace K {
namespace Core {

//! Interface to streams providing blocking item reading.
class ItemReadInterface : public virtual ReadableStreamInterface {
  public:
    //! Reads a binary item of specified size (in bytes).
    /*!
     *  Blocks until either the read is complete or it fails. In case of failure, the output item will be undefined and
     *  error state or EOF will be raised on the stream (or both).
     */
    virtual void ReadItem(void *outItem, int itemSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ITEMREADINTERFACE_H_
