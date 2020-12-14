#ifndef K_IO_BLOCKINGSTREAMINTERFACE_H_
#define K_IO_BLOCKINGSTREAMINTERFACE_H_

#include <K/IO/StreamInterface.h>
#include <K/IO/GenericReadInterface.h>
#include <K/IO/GenericWriteInterface.h>
#include <K/IO/ItemReadInterface.h>
#include <K/IO/ItemWriteInterface.h>

namespace K {
//! I/O.
namespace IO {

//! Interface to entities providing blocking stream I/O.
class BlockingStreamInterface : public virtual StreamInterface,
                                public virtual GenericReadInterface,
                                public virtual GenericWriteInterface,
                                public virtual ItemReadInterface,
                                public virtual ItemWriteInterface {
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BLOCKINGSTREAMINTERFACE_H_
