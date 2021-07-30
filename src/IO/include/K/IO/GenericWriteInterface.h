#ifndef K_IO_GENERICWRITEINTERFACE_H_
#define K_IO_GENERICWRITEINTERFACE_H_

#include <K/Core/StreamInterface.h>

namespace K {
namespace IO {

//! Interface to streams providing generic stream writing.
class GenericWriteInterface : public virtual K::Core::StreamInterface {
  public:
    //! Writes the specified data to the stream.
    /*!
     *  \return
     *  Number of bytes actually written. Might be less than the data size (short write). <c>0</c> does not necessarily
     *  indicate an error.
     */
    virtual int Write(const void *data, int dataSize) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_GENERICWRITEINTERFACE_H_
