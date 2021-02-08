#ifndef K_IO_ITEMREADINTERFACE_H_
#define K_IO_ITEMREADINTERFACE_H_

#include <K/Core/ErrorStateInterface.h>

namespace K {
namespace IO {

//! Interface to entities providing blocking item reading.
class ItemReadInterface : public virtual K::Core::ErrorStateInterface {
  public:
    //! Reads a binary item of specified size (in bytes).
    /*!
     *  Blocks until either the read is complete or it fails.
     *
     * \return
     *  <c>false</c> in case of failure. The output item will then be undefined, and error state or EOF will be raised
     *  on the stream (or both).
     */
    virtual bool ReadItem(void *outItem, int itemSize) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_ITEMREADINTERFACE_H_
