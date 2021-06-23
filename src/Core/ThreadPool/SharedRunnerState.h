#ifndef K_CORE_THREADPOOL_SHAREDRUNNERSTATE_H_
#define K_CORE_THREADPOOL_SHAREDRUNNERSTATE_H_

#include <memory>
#include <mutex>
#include <condition_variable>
#include <K/Core/ThreadPool.h>

namespace K {
namespace Core {

class CompletionHandlerInterface;

//! Shared state for a thread pool runner.
/*!
 *  Threadsafe (all public methods).
 */
class ThreadPool::SharedRunnerState {
  public:
    SharedRunnerState();
    bool WaitForWork(ActionInterface **outAction, CompletionHandlerInterface **outCompletionHandler,
                     int *outCompletionId);
    void Execute(ActionInterface *action, CompletionHandlerInterface *completionHandler, int completionId);
    void RequestShutDown();

  private:
    std::mutex                 lock_;
    std::condition_variable    stateChanged_;
    ActionInterface            *action_;               // Present <=> exec request.
    CompletionHandlerInterface *completionHandler_;    // Valid <=> exec request.
    int                        completionId_;          // Valid <=> exec request.
    bool                       shutDownRequested_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_THREADPOOL_SHAREDRUNNERSTATE_H_

