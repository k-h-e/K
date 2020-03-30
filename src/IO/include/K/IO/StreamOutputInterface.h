#ifndef K_IO_STREAMOUTPUTINTERFACE_H_
#define K_IO_STREAMOUTPUTINTERFACE_H_

#include <K/Core/ErrorInterface.h>

namespace K {
namespace IO {

//! Interface to entities providing stream output.
/*!
 *  \ingroup IO
 */
class StreamOutputInterface : public virtual K::Core::ErrorInterface {
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

#endif    // K_IO_STREAMOUTPUTINTERFACE_H_
