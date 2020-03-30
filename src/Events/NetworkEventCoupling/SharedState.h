#ifndef K_EVENTS_NETWORKEVENTCOUPLING_SHAREDSTATE_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_SHAREDSTATE_H_

#include <mutex>
#include <condition_variable>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/Events/NetworkEventCoupling.h>

namespace K {
namespace Events {

//! State shared between threads of the network event coupling.
/*!
 *  \ingroup Events
 *
 *  The class is thread-safe (i.e. all public methods).
 */
class NetworkEventCoupling::SharedState : public virtual K::Core::CompletionHandlerInterface {
  public:
    SharedState(const std::shared_ptr<K::Core::CompletionHandlerInterface> &completionHandler,
                int completionId);
    void WaitForThreadsFinished();
    void OnCompletion(int completionId);

  private:
    std::mutex                                           lock_;
    std::condition_variable                              stateChanged_;
    std::shared_ptr<K::Core::CompletionHandlerInterface> completionHandler_;
    int                                                  completionId_;
    bool                                                 readerFinished_;
    bool                                                 writerFinished_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_SHAREDSTATE_H_
