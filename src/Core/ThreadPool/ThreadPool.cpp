#include <K/Core/ThreadPool.h>

#include "SharedState.h"

using std::shared_ptr;
using std::make_shared;
using K::Core::ActionInterface;
using K::Core::CompletionHandlerInterface;

namespace K {
namespace Core {

ThreadPool::ThreadPool() {
    sharedState_ = make_shared<SharedState>();
}

ThreadPool::~ThreadPool() {
    sharedState_->ShutDown();
}

void ThreadPool::Run(ActionInterface *action, CompletionHandlerInterface *completionHandler, int completionId) {
    sharedState_->Run(action, completionHandler, completionId);
}

}    // Namespace Core.
}    // Namespace K.
