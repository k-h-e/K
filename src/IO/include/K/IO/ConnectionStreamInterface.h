#ifndef K_IO_CONNECTIONSTREAMINTERFACE_H_
#define K_IO_CONNECTIONSTREAMINTERFACE_H_

#include <K/Core/ItemWriteInterface.h>
#include <K/IO/AsyncReadInterface.h>

namespace K {
namespace IO {

//! Interface to connection streams.
class ConnectionStreamInterface : public virtual Core::ItemWriteInterface,
                                  public virtual AsyncReadInterface {
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONNECTIONSTREAMINTERFACE_H_
