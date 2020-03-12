#ifndef K_CORE_THREADPOOL_SHAREDSTATE_H_
#define K_CORE_THREADPOOL_SHAREDSTATE_H_

#include <vector>
#include <memory>
#include <thread>
#include <condition_variable>
#include <unordered_set>
#include <K/Core/ThreadPool.h>
#include <K/Core/CompletionHandlerInterface.h>

namespace kxm {
namespace Core {
    class ActionInterface;
}
}

namespace K {
namespace Core {

class CompletionHandlerInterface;

//! Shared state for the thread pool.
/*!
 *  \ingroup Core
 *
 *  Thread-safe (all public methods).
 */
class ThreadPool::SharedState : public virtual K::Core::CompletionHandlerInterface {
  public:
    void Run(const std::shared_ptr<kxm::Core::ActionInterface> &action,
             const std::shared_ptr<K::Core::CompletionHandlerInterface> &completionHandler);
    void OnCompletion(int operationId);
    void ShutDown();

  private:
    struct ThreadInfo {
        std::unique_ptr<std::thread>       thread;
        std::shared_ptr<Runner>            runner;
        std::shared_ptr<SharedRunnerState> sharedRunnerState;
    };

    std::mutex              lock_;
    std::condition_variable stateChanged_;
    std::vector<ThreadInfo> threads_;
    std::unordered_set<int> idleThreads_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_THREADPOOL_SHAREDSTATE_H_