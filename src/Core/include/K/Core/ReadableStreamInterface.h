#ifndef K_CORE_READABLESTREAMINTERFACE_H_
#define K_CORE_READABLESTREAMINTERFACE_H_

#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

//! Interface to streams that are readable.
class ReadableStreamInterface : public virtual StreamInterface {
  public:
    //! Tells whether the stream is in EOF state because a read operation attempted earlier could not deliver data
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

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_READABLESTREAMINTERFACE_H_
