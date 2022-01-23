////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_THREADPOOL_H_
#define K_CORE_THREADPOOL_H_

#include <memory>

namespace K {
namespace Core {

class ActionInterface;
class CompletionHandlerInterface;

//! Thread pool.
/*!
 *  Thread-safe (all public methods).
 */
class ThreadPool {
  public:
    ThreadPool();
    ThreadPool(const ThreadPool &other)            = delete;
    ThreadPool &operator=(const ThreadPool &other) = delete;
    ThreadPool(ThreadPool &&other)                 = delete;
    ThreadPool &operator=(ThreadPool &&other)      = delete;
    ~ThreadPool();

    //! Runs the specified action on an arbitrary pool thread.
    /*!
     *  The caller is responsible for ensuring that both action and completion handler live until the thread pool calls
     *  the completion handler to signal completion of the action.
     *
     *  \param completionHandler
     *  The completion handler will get called on an arbitrary pool thread when the action has completed.
     *
     *  \param completionId
     *  Will get passed to the completion handler. Allows a single completion handler to be used with multiple requested
     *  threads.
     */
    void Run(ActionInterface *action, CompletionHandlerInterface *completionHandler, int completionId);

  private:
    class SharedState;
    class Runner;
    class SharedRunnerState;

    std::shared_ptr<SharedState> sharedState_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_THREADPOOL_H_
