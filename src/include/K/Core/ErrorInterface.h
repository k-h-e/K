#ifndef K_CORE_ERRORINTERFACE_H_
#define K_CORE_ERRORINTERFACE_H_

#include <kxm/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to entities that can enter error state.
/*!
 *  \ingroup Core
 */
class ErrorInterface : public virtual kxm::Core::Interface {
  public:
    //! Tells whether or not the object has entered error state.
    /*!
     *  When in error state, all operations on the object have no effect and indicate failure (for operations returning
     *  results).
     *
     *  Once the object has entered error state, it does not automatically leave it again.
     */
    virtual bool Error() = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ERRORINTERFACE_H_
