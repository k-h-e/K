/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

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
    CompletionActionAdapter()                                                = delete;
    CompletionActionAdapter(const CompletionActionAdapter &other)            = delete;
    CompletionActionAdapter &operator=(const CompletionActionAdapter &other) = delete;
    CompletionActionAdapter(CompletionActionAdapter &&other)                 = delete;
    CompletionActionAdapter &operator=(CompletionActionAdapter &&other)      = delete;
    ~CompletionActionAdapter()                                               = default;

    // CompletionHandlerInterface...
    void OnCompletion(int completionId) override;

  private:
    std::shared_ptr<ActionInterface> action_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_COMPLETIONACTIONADAPTER_H_
