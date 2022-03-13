#ifndef K_CORE_SEEKABLEBLOCKINGIOSTREAMINTERFACE_H_
#define K_CORE_SEEKABLEBLOCKINGIOSTREAMINTERFACE_H_

#include <K/Core/BlockingIOStreamInterface.h>
#include <K/Core/SeekableBlockingInStreamInterface.h>

namespace K {
namespace Core {

//! Interface to seekable blocking input/output streams.
class SeekableBlockingIOStreamInterface : public virtual BlockingIOStreamInterface,
                                          public virtual SeekableBlockingInStreamInterface {
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SEEKABLEBLOCKINGIOSTREAMINTERFACE_H_