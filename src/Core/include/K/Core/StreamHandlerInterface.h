#ifndef K_CORE_STREAMHANDLERINTERFACE_H_
#define K_CORE_STREAMHANDLERINTERFACE_H_

#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

//! Interface to entities accepting and processing stream data.
class StreamHandlerInterface : public virtual Interface {
  public:
    //! Hands over another portion of stream data to the handler.
    /*!
     *  \param id
     *  Activation ID that was registered together with the handler.
     */
    virtual void OnStreamData(int id, const void *data, int dataSize) = 0;
    //! Informs the handler that the stream has entered error state, see <c>StreamInterface::ErrorState()</c>.
    /*!
     *  \param id
     *  Activation ID that was registered together with the handler.
     */
    virtual void OnStreamEnteredErrorState(int id, StreamInterface::Error error) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_STREAMHANDLERINTERFACE_H_

