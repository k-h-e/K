/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_COMPLETIONTRACKER_H_
#define K_CORE_COMPLETIONTRACKER_H_

#include <mutex>
#include <condition_variable>
#include <unordered_set>
#include <K/Core/CompletionHandlerInterface.h>

namespace K {
namespace Core {

//! Keeps track of completion states.
/*!
 *  The class is thread-safe, i.e. all its public instance methods are.
 */
class CompletionTracker : public virtual CompletionHandlerInterface {
  public:
    CompletionTracker()                                          = default;
    CompletionTracker(const CompletionTracker &other)            = delete;
    CompletionTracker &operator=(const CompletionTracker &other) = delete;
    CompletionTracker(CompletionTracker &&other)                 = delete;
    CompletionTracker &operator=(CompletionTracker &&other)      = delete;

    //! Blocks until the specified completion state has been set.
    void WaitForCompletion(int completionId);
    void OnCompletion(int completionId) override;

  private:
    std::mutex              lock_;
    std::condition_variable stateChanged_;
    std::unordered_set<int> setCompletionStates_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_COMPLETIONTRACKER_H_
