#ifndef K_CORE_ITEMWRITEINTERFACE_H_
#define K_CORE_ITEMWRITEINTERFACE_H_

#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

//! Interface to streams providing blocking item writing.
class ItemWriteInterface : public virtual StreamInterface {
  public:
    //! Writes a binary item of specified size (in bytes).
    /*!
     *  Blocks until either the write is complete or it fails. In case of failure, error state will be raised on the
     *  stream.
     */
    virtual void WriteItem(const void *item, int itemSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ITEMWRITEINTERFACE_H_
