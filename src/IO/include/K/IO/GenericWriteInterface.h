#ifndef K_IO_GENERICWRITEINTERFACE_H_
#define K_IO_GENERICWRITEINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
//! I/O.
namespace IO {

//! Interface to entities providing generic stream writing.
class GenericWriteInterface : public virtual K::Core::Interface {
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
