#ifndef K_CORE_DATAHANDLERINTERFACE_H_
#define K_CORE_DATAHANDLERINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to entities accepting and processing data.
class DataHandlerInterface : public virtual Interface {
  public:
    //! Gets called once to indicate that there will be no more data because the source has reached EOF.
    /*!
     *  \param id
     *  Activation ID that was registered together with the handler.
     */
    virtual void HandleEof(int id) = 0;
    //! Gets called once to indicate that there will be no more data because the source has entered error state (see
    //! <c>K::Core::ErrorInterface</c>).
    /*!
     *  \param id
     *  Activation ID that was registered together with the handler.
     */
    virtual void HandleError(int id) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_DATAHANDLERINTERFACE_H_
