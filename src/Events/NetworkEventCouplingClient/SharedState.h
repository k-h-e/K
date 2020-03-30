#ifndef K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_SHAREDSTATE_H_
#define K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_SHAREDSTATE_H_

#include <memory>
#include <mutex>
#include <condition_variable>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/Events/NetworkEventCoupling.h>
#include <K/Events/NetworkEventCouplingClient.h>

namespace K {
namespace Events {

//! State shared between threads of the network event coupling client.
/*!
 *  \ingroup Events
 *
 *  The class is thread-safe (i.e. all public methods).
 */
class NetworkEventCouplingClient::SharedState : public virtual K::Core::CompletionHandlerInterface {
  public:
    SharedState();
    void PrepareToConnect();
    void WaitForWorker();
    void Disconnect();
    void OnCouplingCreated(const std::shared_ptr<NetworkEventCoupling> &coupling);
    void OnCompletion(int completionId);

  private:
    std::mutex                            lock_;
    std::condition_variable               stateChanged_;
    std::shared_ptr<NetworkEventCoupling> coupling_;
    bool                                  connecting_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLINGCLIENT_SHAREDSTATE_H_
