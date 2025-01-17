/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/CompletionActionAdapter.h>

#include <K/Core/ActionInterface.h>

using std::shared_ptr;
using K::Core::ActionInterface;

namespace K {
namespace Core {

CompletionActionAdapter::CompletionActionAdapter(const shared_ptr<ActionInterface> &action)
        : action_{action} {
    // Nop.
}

void CompletionActionAdapter::OnCompletion(int completionId) {
    (void)completionId;
    action_->Execute();
}

}    // Namespace Core.
}    // Namespace K.
