#ifndef K_CORE_CONNECTIONSTREAMINTERFACE_H_
#define K_CORE_CONNECTIONSTREAMINTERFACE_H_

#include <K/Core/AsyncReadInterface.h>
#include <K/Core/EofInterface.h>
#include <K/Core/ItemWriteInterface.h>
#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

//! Interface to connection streams.
class ConnectionStreamInterface : public virtual StreamInterface,
                                  public virtual EofInterface,
                                  public virtual ItemWriteInterface,
                                  public virtual AsyncReadInterface {
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_CONNECTIONSTREAMINTERFACE_H_
