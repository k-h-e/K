#ifndef K_IO_EOFINTERFACE_H_
#define K_IO_EOFINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace IO {

//! Interface to streams able to tell whether or not reading has reached the end of the stream.
class EofInterface : public virtual K::Core::Interface {
  public:
    //! Tells whether the stream is in EOF state because a read operation attempted earlier could not deliver any data
    //! because the end of the stream has been reached.
    /*!
     *  While the stream has EOF state set, read operations are disabled and respective methods report failure where
     *  appropriate.
     *
     *  Once EOF state has been raised on the stream, it remains that way until explicitly cleared via ClearEof().
     */
    virtual bool Eof() = 0;
    //! Clears EOF state (in case it was set).
    virtual void ClearEof() = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_EOFINTERFACE_H_
