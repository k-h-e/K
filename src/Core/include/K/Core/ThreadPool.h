/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_THREADPOOL_H_
#define K_CORE_THREADPOOL_H_

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

class ActionInterface;
class CompletionHandlerInterface;

//! Thread pool.
/*!
 *  Thread-safe (all public methods).
 */
class ThreadPool : public virtual Interface {
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
    struct ActionInfo {
        ActionInterface            *action;
        CompletionHandlerInterface *completionHandler;
        int                        completionId;

        ActionInfo()                                   = delete;
        ActionInfo(ActionInterface *anAction, CompletionHandlerInterface *aCompletionHandler, int aCompletionId)
            : action(anAction),
              completionHandler(aCompletionHandler),
              completionId(aCompletionId) {}
        ActionInfo(const ActionInfo &other)            = default;
        ActionInfo &operator=(const ActionInfo &other) = default;
        ActionInfo(ActionInfo &&other)                 = default;
        ActionInfo &operator=(ActionInfo &&other)      = default;
    };
    struct ThreadInfo {
        std::thread               thread;
        std::condition_variable   stateChanged;
        bool                      shutDownRequested;
        std::optional<ActionInfo> actionToExecute;
        bool                      finished;
        int                       threadId;
        ThreadPool                *threadPool;

        ThreadInfo()                                   = delete;
        ThreadInfo(int aThread, ThreadPool *aThreadPool);
        ThreadInfo(const ThreadInfo &other)            = delete;
        ThreadInfo &operator=(const ThreadInfo &other) = delete;
        ThreadInfo(ThreadInfo &&other)                 = delete;
        ThreadInfo &operator=(ThreadInfo &&other)      = delete;
        ~ThreadInfo();
    };

    std::optional<const ActionInfo> WaitForAction(int thread);

    static void ThreadMain(int thread, ThreadPool *threadPool);

    std::mutex                               lock_;            // Protects everything below...
    std::condition_variable                  stateChanged_;
    std::vector<std::unique_ptr<ThreadInfo>> threads_;
    std::deque<int>                          idleThreads_;
    std::deque<int>                          freeSlots_;
    bool                                     actionDispatchUnderway_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_THREADPOOL_H_
