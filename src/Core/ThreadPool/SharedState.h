////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_THREADPOOL_SHAREDSTATE_H_
#define K_CORE_THREADPOOL_SHAREDSTATE_H_

#include <vector>
#include <memory>
#include <thread>
#include <condition_variable>
#include <unordered_set>
#include <K/Core/ThreadPool.h>
#include <K/Core/CompletionHandlerInterface.h>

namespace K {
namespace Core {

class ActionInterface;
class CompletionHandlerInterface;

//! Shared state for the thread pool.
/*!
 *  Thread-safe (all public methods).
 */
class ThreadPool::SharedState : public virtual CompletionHandlerInterface {
  public:
    SharedState()                                    = default;
    SharedState(const SharedState &other)            = delete;
    SharedState &operator=(const SharedState &other) = delete;
    SharedState(SharedState &&other)                 = delete;
    SharedState &operator=(SharedState &&other)      = delete;

    void Run(ActionInterface *action, CompletionHandlerInterface *completionHandler, int completionId);
    void OnCompletion(int completionId);
    void ShutDown();

  private:
    struct ThreadInfo {
        std::unique_ptr<std::thread>       thread;
        std::shared_ptr<Runner>            runner;
        std::shared_ptr<SharedRunnerState> sharedRunnerState;

        ThreadInfo()                                   = default;
        ThreadInfo(const ThreadInfo &other)            = delete;
        ThreadInfo &operator=(const ThreadInfo &other) = delete;
        ThreadInfo(ThreadInfo &&other)                 = default;
        ThreadInfo &operator=(ThreadInfo &&other)      = default;
    };

    void LogStats();

    std::mutex              lock_;
    std::condition_variable stateChanged_;
    std::vector<ThreadInfo> threads_;
    std::unordered_set<int> idleThreads_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_THREADPOOL_SHAREDSTATE_H_
