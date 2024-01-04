/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/ThreadPool.h>

#include <cassert>
#include <K/Core/ActionInterface.h>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/Core/Log.h>

using std::deque;
using std::make_unique;
using std::mutex;
using std::nullopt;
using std::optional;
using std::to_string;
using std::unique_lock;
using std::unique_ptr;
using K::Core::Log;

namespace K {
namespace Core {

ThreadPool::ThreadPool()
        : actionDispatchUnderway_(false) {
    // Nop.
}

ThreadPool::~ThreadPool() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    int numRunning = 0;
    for (unique_ptr<ThreadInfo> &info : threads_) {
        if (info) {
            ++numRunning;
            info->shutDownRequested = true;
            info->stateChanged.notify_all();
        }
    }

    while (numRunning) {
        Log::Print(Log::Level::Debug, this, [&]{
            return "waiting for " + to_string(numRunning) + " threads to terminate...";
        });
        stateChanged_.wait(critical);

        numRunning = 0;
        for (unique_ptr<ThreadInfo> &info : threads_) {
            if (info) {
                if (info->finished) {
                    freeSlots_.push_back(info->threadId);    // Keep it clean, even at the end.
                    info.reset();                            // Joins the thread.
                } else {
                    ++numRunning;
                }
            }
        }
    }

    Log::Print(Log::Level::Debug, this, [&]{ return "all threads terminated"; });

}    // ......................................................................................... critical section, end.

void ThreadPool::Run(ActionInterface *action, CompletionHandlerInterface *completionHandler, int completionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    while (actionDispatchUnderway_) {
        stateChanged_.wait(critical);
    }
    actionDispatchUnderway_ = true;

    if (idleThreads_.empty()) {
        int newThread;
        if (!freeSlots_.empty()) {
            newThread = freeSlots_.front();
            freeSlots_.pop_front();
        } else {
            newThread = static_cast<int>(threads_.size());
            threads_.push_back(unique_ptr<ThreadInfo>());
        }
        threads_[newThread] = make_unique<ThreadInfo>(newThread, this);
        Log::Print(Log::Level::Debug, this, [&]{
            return "added new thread " + to_string(newThread) + ", num_threads="
                + to_string(threads_.size() - freeSlots_.size());
        });

        // Wait for newly created thead to appear as idle...
        while (idleThreads_.empty()) {
            stateChanged_.wait(critical);
        }
    }

    int thread = idleThreads_.front();
    idleThreads_.pop_front();
    unique_ptr<ThreadInfo> &info = threads_[thread];
    assert(info);
    info->actionToExecute = ActionInfo(action, completionHandler, completionId);
    info->stateChanged.notify_all();
    Log::Print(Log::Level::Debug, this, [&]{
        return "action dispatched onto thread " + to_string(thread) + ", num_idle=" + to_string(idleThreads_.size());
    });

    actionDispatchUnderway_ = false;

}    // ......................................................................................... critical section, end.

optional<const ThreadPool::ActionInfo> ThreadPool::WaitForAction(int thread) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    idleThreads_.push_back(thread);
    stateChanged_.notify_all();
    Log::Print(Log::Level::Debug, this, [&]{
        return "thread " + to_string(thread) + " now idle, num_idle=" + to_string(idleThreads_.size());
    });
    while (true) {
        unique_ptr<ThreadInfo> &info = threads_[thread];
        assert(info);
        if (info->shutDownRequested) {
            info->finished = true;
            stateChanged_.notify_all();
            return nullopt;
        } else if (info->actionToExecute) {
            optional<ActionInfo> action = info->actionToExecute;
            info->actionToExecute.reset();
            return action;
        } else {
            info->stateChanged.wait(critical);
        }
    }
}    // ...................................................................................... critical section, end.

void ThreadPool::ThreadMain(int thread, ThreadPool *threadPool) {
    Log::Print(Log::Level::Debug, threadPool, [&]{ return "thread " + to_string(thread) + " spawning..."; });

    bool done = false;
    while (!done) {
        optional<const ThreadPool::ActionInfo> actionInfo = threadPool->WaitForAction(thread);
        if (actionInfo) {
            actionInfo->action->ExecuteAction();
            actionInfo->completionHandler->OnCompletion(actionInfo->completionId);
        } else {
            done = true;
        }
    }

    Log::Print(Log::Level::Debug, threadPool, [&]{ return "thread " + to_string(thread) + " exiting"; });
}

ThreadPool::ThreadInfo::ThreadInfo(int aThread, ThreadPool *aThreadPool)
        : thread(ThreadPool::ThreadMain, aThread, aThreadPool),
          shutDownRequested(false),
          finished(false),
          threadId(aThread),
          threadPool(aThreadPool) {
    // Nop.
}

ThreadPool::ThreadInfo::~ThreadInfo() {
    thread.join();
    Log::Print(Log::Level::Debug, threadPool, [&]{ return "thread " + to_string(threadId) + " terminated"; });
}

}    // Namespace Core.
}    // Namespace K.
