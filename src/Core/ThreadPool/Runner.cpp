////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Runner.h"

#include <K/Core/ActionInterface.h>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/Core/Log.h>
#include "SharedRunnerState.h"

using std::shared_ptr;
using std::to_string;
using K::Core::ActionInterface;
using K::Core::CompletionHandlerInterface;
using K::Core::Log;

namespace K {
namespace Core {

ThreadPool::Runner::Runner(int thread, CompletionHandlerInterface &completionHandler,
                           const shared_ptr<SharedRunnerState> &sharedState)
        : sharedState_(sharedState),
          thread_(thread),
          completionHandler_(completionHandler) {
    // Nop.
}

void ThreadPool::Runner::Run() {
    Log::Print(Log::Level::Debug, this, [this]{ return "thread " + to_string(thread_) + " spawning..."; });

    ActionInterface            *action;
    CompletionHandlerInterface *actionCompletionHandler;
    int                        completionId;
    while (sharedState_->WaitForWork(&action, &actionCompletionHandler, &completionId)) {
        action->ExecuteAction();
        actionCompletionHandler->OnCompletion(completionId);

        completionHandler_.OnCompletion(thread_);
    }

    Log::Print(Log::Level::Debug, this, [this]{ return "thread " + to_string(thread_) + " terminating"; });
}

}    // Namespace Core.
}    // Namespace K.

