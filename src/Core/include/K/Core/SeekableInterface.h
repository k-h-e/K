#ifndef K_CORE_SEEKABLEINTERFACE_H_
#define K_CORE_SEEKABLEINTERFACE_H_

#include <cstdint>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to data readers and writers providing seeking.
class SeekableInterface : public virtual Interface {
  public:
    //! Sets the read/write position as specified.
    virtual void Seek(int64_t position) = 0;
    //! Returns the current read/write position.
    virtual int64_t StreamPosition() const = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SEEKABLEINTERFACE_H_
