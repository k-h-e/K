#ifndef K_CORE_COMPLETIONACTIONADAPTER_H_
#define K_CORE_COMPLETIONACTIONADAPTER_H_

#include <memory>
#include <K/Core/CompletionHandlerInterface.h>

namespace K {
namespace Core {

class ActionInterface;

//! Completion handler executing an action.
class CompletionActionAdapter : public virtual CompletionHandlerInterface {
  public:
    CompletionActionAdapter(const std::shared_ptr<ActionInterface> &action);
    virtual void OnCompletion(int completionId);

  private:
    std::shared_ptr<ActionInterface> action_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_COMPLETIONACTIONADAPTER_H_
