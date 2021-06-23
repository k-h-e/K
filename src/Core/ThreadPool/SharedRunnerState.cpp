#include "SharedRunnerState.h"

using std::shared_ptr;
using std::mutex;
using std::unique_lock;
using K::Core::ActionInterface;
using K::Core::CompletionHandlerInterface;

namespace K {
namespace Core {

ThreadPool::SharedRunnerState::SharedRunnerState()
        : completionId_(0),
          shutDownRequested_(false) {
    // Nop.
}

bool ThreadPool::SharedRunnerState::WaitForWork(
        ActionInterface **outAction, CompletionHandlerInterface **outCompletionHandler, int *outCompletionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    while (!shutDownRequested_) {
        if (action_) {
            *outAction            = action_;
            *outCompletionHandler = completionHandler_;
            *outCompletionId      = completionId_;
            action_            = nullptr;
            completionHandler_ = nullptr;
            return true;
        }

        stateChanged_.wait(critical);
    }

    return false;
}    // ......................................................................................... critical section, end.

void ThreadPool::SharedRunnerState::Execute(
        ActionInterface *action, CompletionHandlerInterface *completionHandler, int completionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    action_            = action;
    completionHandler_ = completionHandler;
    completionId_      = completionId;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

void ThreadPool::SharedRunnerState::RequestShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    shutDownRequested_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

}    // Namespace Core.
}    // Namespace K.

