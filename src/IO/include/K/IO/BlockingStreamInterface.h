#ifndef K_IO_BLOCKINGSTREAMINTERFACE_H_
#define K_IO_BLOCKINGSTREAMINTERFACE_H_

#include <K/IO/StreamInterface.h>

namespace K {
//! I/O.
namespace IO {

//! Interface to entities providing blocking stream I/O.
class BlockingStreamInterface : public virtual StreamInterface {
  public:
    //! Reads a binary item of specified size (in bytes).
    /*!
     * \return
     *  <c>false</c> in case of failure. The output item will then be undefined, and error state will be raised on the
     *  stream.
     */
    virtual bool ReadItem(void *outItem, int itemSize) = 0;
    //! Writes a binary item of specified size (in bytes).
    /*!
     * \return
     *  <c>false</c> in case of failure. Error state will then be raised on the stream.
     */
    virtual bool WriteItem(const void *item, int itemSize) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BLOCKINGSTREAMINTERFACE_H_
