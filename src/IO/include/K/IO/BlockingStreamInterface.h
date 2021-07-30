#ifndef K_IO_BLOCKINGSTREAMINTERFACE_H_
#define K_IO_BLOCKINGSTREAMINTERFACE_H_

#include <K/Core/ItemReadInterface.h>
#include <K/Core/ItemWriteInterface.h>
#include <K/IO/GenericReadInterface.h>
#include <K/IO/GenericWriteInterface.h>

namespace K {
//! I/O.
namespace IO {

//! Interface to entities providing blocking stream I/O.
class BlockingStreamInterface : public virtual GenericReadInterface,
                                public virtual GenericWriteInterface,
                                public virtual Core::ItemReadInterface,
                                public virtual Core::ItemWriteInterface {
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BLOCKINGSTREAMINTERFACE_H_
