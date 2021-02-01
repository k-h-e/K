#ifndef K_IO_EOFINTERFACE_H_
#define K_IO_EOFINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace IO {

//! Interface to streams able to tell whether or not reading has reached the end of the stream.
class EofInterface : public virtual K::Core::Interface {
  public:
    //! Tells whether or not reading has reached the end of the stream.
    virtual bool Eof() = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_EOFINTERFACE_H_
