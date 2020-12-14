#ifndef K_IO_CONNECTIONSTREAMINTERFACE_H_
#define K_IO_CONNECTIONSTREAMINTERFACE_H_

#include <K/IO/StreamInterface.h>
#include <K/IO/ItemWriteInterface.h>
#include <K/IO/AsyncReadInterface.h>

namespace K {
namespace IO {

//! Interface to connection streams.
class ConnectionStreamInterface : public virtual StreamInterface,
                                  public virtual ItemWriteInterface,
                                  public virtual AsyncReadInterface {
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONNECTIONSTREAMINTERFACE_H_
