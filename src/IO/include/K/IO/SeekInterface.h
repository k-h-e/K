#ifndef K_IO_SEEKINTERFACE_H_
#define K_IO_SEEKINTERFACE_H_

#include <cstdint>
#include <K/Core/Interface.h>

namespace K {
namespace IO {

//! Interface to entities providing stream seeking.
class SeekInterface : public virtual K::Core::Interface {
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

#endif    // K_IO_SEEKINTERFACE_H_
