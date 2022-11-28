#ifndef K_CORE_CONNECTIONSTREAMINTERFACE_H_
#define K_CORE_CONNECTIONSTREAMINTERFACE_H_

#include <K/Core/AsyncInStreamInterface.h>
#include <K/Core/BlockingOutStreamInterface.h>

namespace K {
namespace Core {

//! Interface to connection streams.
class ConnectionStreamInterface : public virtual AsyncInStreamInterface,
                                  public virtual BlockingOutStreamInterface {
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_CONNECTIONSTREAMINTERFACE_H_
