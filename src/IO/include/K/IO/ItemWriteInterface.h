#ifndef K_IO_ITEMWRITEINTERFACE_H_
#define K_IO_ITEMWRITEINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace IO {

//! Interface to entities providing blocking item writing.
class ItemWriteInterface : public virtual K::Core::Interface {
  public:
    //! Writes a binary item of specified size (in bytes).
    /*!
     *  Blocks until either the write is complete or it fails.
     *
     *  \return
     *  <c>false</c> in case of failure. Error state will then be raised on the stream.
     */
    virtual bool WriteItem(const void *item, int itemSize) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_ITEMWRITEINTERFACE_H_