#ifndef K_CORE_THREADPOOL_RUNNER_H_
#define K_CORE_THREADPOOL_RUNNER_H_

#include <memory>
#include <K/Core/ThreadPool.h>

namespace K {
namespace Core {

//! Runner for the thread pool.
/*!
 *  \ingroup Core
 */
class ThreadPool::Runner {
  public:
    Runner(int thread, CompletionHandlerInterface &completionHandler,
           const std::shared_ptr<SharedRunnerState> &sharedState);
    void Run();

  private:
    std::shared_ptr<SharedRunnerState> sharedState_;

    int                        thread_;
    CompletionHandlerInterface &completionHandler_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_THREADPOOL_RUNNER_H_
