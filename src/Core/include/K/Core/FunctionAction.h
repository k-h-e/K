#ifndef K_CORE_FUNCTIONACTION_H_
#define K_CORE_FUNCTIONACTION_H_

#include <functional>
#include <K/Core/ActionInterface.h>

namespace K {
namespace Core {

//! Action calling a <c>std::function</c>.
/*!
 *  \ingroup Core
 */
class FunctionAction : public virtual ActionInterface {
  public:
    FunctionAction(const std::function<void()> &aFunction);
    virtual void ExecuteAction();

  private:
    std::function<void()> function_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_FUNCTIONACTION_H_
