#ifndef K_IO_STREAMINTERFACE_H_
#define K_IO_STREAMINTERFACE_H_

#include <K/Core/ErrorInterface.h>

namespace K {
//! I/O.
namespace IO {

//! Interface to entities providing stream I/O.
class StreamInterface : public virtual K::Core::ErrorInterface {
  public:
    //! Tells whether or not reading has reached the end of the stream.
    virtual bool Eof() = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMINTERFACE_H_
