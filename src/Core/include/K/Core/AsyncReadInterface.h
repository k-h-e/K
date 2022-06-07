#ifndef K_CORE_ASYNCREADINTERFACE_H_
#define K_CORE_ASYNCREADINTERFACE_H_

#include <memory>
#include <K/Core/StreamHandlerInterface.h>

namespace K {
namespace Core {

//! Interface to data readers providing asynchronous reading.
class AsyncReadInterface : public virtual Interface {
  public:
    //! Registers the specified read handler.
    /*!
     *  The handler methods will get called on an arbitrary thread and must not call back into the stream.
     */
    virtual bool Register(const std::shared_ptr<StreamHandlerInterface> &handler, int activationId) = 0;
    //! Unregisters the specified read handler if it was registered.
    /*!
     *  When the method returns, it is guaranteed that the handler will not be called again. If registering another read
     *  handler later, read data might have been missed - with the exception of the initial time after stream
     *  construction, there is no garantee that the stream buffers read data while no read handler is registered.
     */
    virtual void Unregister(const std::shared_ptr<StreamHandlerInterface> &handler) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ASYNCREADINTERFACE_H_
