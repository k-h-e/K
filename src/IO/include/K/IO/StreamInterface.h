#ifndef K_IO_STREAMINTERFACE_H_
#define K_IO_STREAMINTERFACE_H_

#include <K/Core/ErrorInterface.h>

namespace K {
//! I/O.
namespace IO {

//! Interface to entities providing stream I/O.
class StreamInterface : public virtual K::Core::ErrorInterface {
  public:
    //! Reads data into the specified buffer.
    /*!
     *  \return
     *  Number of bytes actually read. Will not be larger than the buffer size. Might be less than the buffer size
     *  (short read). <c>0</c> does not necessarily indicate an error.
     */
    virtual int Read(void *outBuffer, int bufferSize) = 0;
    //! Writes the specified data to the stream.
    /*!
     *  \return
     *  Number of bytes actually written. Might be less than the data size (short write). <c>0</c> does not necessarily
     *  indicate an error.
     */
    virtual int Write(const void *data, int dataSize) = 0;
    //! Tells whether or not reading has reached the end of the stream.
    virtual bool EndOfStream() = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMINTERFACE_H_
