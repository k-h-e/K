/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/CompletionTracker.h>

using std::mutex;
using std::unique_lock;

namespace K {
namespace Core {

void CompletionTracker::WaitForCompletion(int completionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    while (setCompletionStates_.find(completionId) == setCompletionStates_.end()) {
        stateChanged_.wait(critical);
    }
}    // ......................................................................................... critical section, end.

void CompletionTracker::OnCompletion(int completionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    setCompletionStates_.insert(completionId);
    stateChanged_.notify_all();
}

}    // Namespace Core.
}    // Namespace K.
