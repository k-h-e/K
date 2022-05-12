////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_FRAMEWORK_INTERACTIONCONNECTIONENDPOINT_H_
#define K_IO_FRAMEWORK_INTERACTIONCONNECTIONENDPOINT_H_

#include <memory>
#include <K/Core/Interface.h>

namespace K {
    namespace Core {
        namespace Framework {
            class NonBlockingIOStreamInterface;
        }
    }
}

namespace K {
namespace IO {
namespace Framework {

//! Convenience endpoint for nonblocking connection streams that are operated in an interaction-like fashion.
class InteractionConnectionEndPoint : public virtual Core::Interface {
  public:
    InteractionConnectionEndPoint(const std::shared_ptr<Core::Framework::NonBlockingIOStreamInterface> &connection);
    InteractionConnectionEndPoint()                                                      = delete;
    InteractionConnectionEndPoint(const InteractionConnectionEndPoint &other)            = delete;
    InteractionConnectionEndPoint &operator=(const InteractionConnectionEndPoint &other) = delete;
    InteractionConnectionEndPoint(InteractionConnectionEndPoint &&other)                 = delete;
    InteractionConnectionEndPoint &operator=(InteractionConnectionEndPoint &&other)      = delete;
    ~InteractionConnectionEndPoint()                                                     = default;

  private:
    const std::shared_ptr<Core::Framework::NonBlockingIOStreamInterface> connection_;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_WRITEBUFFER_H_
