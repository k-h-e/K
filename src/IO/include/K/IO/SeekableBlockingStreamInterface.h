#ifndef K_IO_SEEKABLEBLOCKINGSTREAMINTERFACE_H_
#define K_IO_SEEKABLEBLOCKINGSTREAMINTERFACE_H_

#include <K/IO/BlockingStreamInterface.h>
#include <K/IO/SeekableInterface.h>

namespace K {
namespace IO {

//! Interface to streams providing seekable blocking I/O.
class SeekableBlockingStreamInterface : public virtual BlockingStreamInterface,
                                        public virtual SeekableInterface {
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_SEEKABLEBLOCKINGSTREAMINTERFACE_H_
