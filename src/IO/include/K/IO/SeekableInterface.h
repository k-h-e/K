#ifndef K_IO_SEEKABLEINTERFACE_H_
#define K_IO_SEEKABLEINTERFACE_H_

#include <cstdint>
#include <K/Core/StreamInterface.h>

namespace K {
namespace IO {

//! Interface to streams providing seeking.
class SeekableInterface : public virtual K::Core::StreamInterface {
  public:
    //! Sets the read/write position as specified.
    /*!
     *  In case of failure, the stream will enter error state.
     */
    virtual void Seek(int64_t position) = 0;
    //! Returns the stream's current read/write position.
    virtual int64_t StreamPosition() = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_SEEKABLEINTERFACE_H_
