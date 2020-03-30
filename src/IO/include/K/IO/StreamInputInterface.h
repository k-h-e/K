#ifndef K_IO_STREAMINPUTINTERFACE_H_
#define K_IO_STREAMINPUTINTERFACE_H_

#include <K/Core/ErrorInterface.h>

namespace K {
namespace IO {

//! Interface to entities providing stream input.
/*!
 *  \ingroup IO
 */
class StreamInputInterface : public virtual K::Core::ErrorInterface {
  public:
    //! Reads data into the specified buffer.
    /*!
     *  \return
     *  Number of bytes actually read. Will not be larger than the buffer size. Might be less than the buffer size
     *  (short read). <c>0</c> does not necessarily indicate an error.
     */
    virtual int Read(void *outBuffer, int bufferSize) = 0;
    //! Tells whether or not reading has reached the end of the stream.
    virtual bool EndOfStream() = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMINPUTINTERFACE_H_
