#ifndef K_IO_SEEKABLEBLOCKINGSTREAMINTERFACE_H_
#define K_IO_SEEKABLEBLOCKINGSTREAMINTERFACE_H_

#include <K/IO/BlockingStreamInterface.h>
#include <K/IO/SeekInterface.h>

namespace K {
namespace IO {

//! Interface to entities providing seekable blocking stream I/O.
class SeekableBlockingStreamInterface : public virtual BlockingStreamInterface,
                                        public virtual SeekInterface {
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_SEEKABLEBLOCKINGSTREAMINTERFACE_H_
