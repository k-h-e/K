#ifndef K_IO_GENERICREADINTERFACE_H_
#define K_IO_GENERICREADINTERFACE_H_

#include <K/Core/ReadableStreamInterface.h>

namespace K {
namespace IO {

//! Interface to streams providing generic stream reading.
class GenericReadInterface : public virtual K::Core::ReadableStreamInterface {
  public:
    //! Reads data into the specified buffer.
    /*!
     *  \return
     *  Number of bytes actually read. Will not be larger than the buffer size. Might be less than the buffer size
     *  (short read). <c>0</c> does not necessarily indicate an error.
     */
    virtual int Read(void *outBuffer, int bufferSize) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_GENERICREADINTERFACE_H_
