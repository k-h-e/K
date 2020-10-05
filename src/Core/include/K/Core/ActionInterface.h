#ifndef K_CORE_ACTIONINTERFACE_H_
#define K_CORE_ACTIONINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to actions that can be executed.
class ActionInterface : public virtual Interface {
  public:
    //! Executes the action.
    virtual void ExecuteAction() = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ACTIONINTERFACE_H_
