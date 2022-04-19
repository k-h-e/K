#ifndef K_EVENTS_NETWORKEVENTCOUPLING_SHAREDSTATE_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_SHAREDSTATE_H_

#include <mutex>
#include <condition_variable>
#include <optional>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/Core/Timers.h>
#include <K/Events/NetworkEventCoupling.h>

namespace K {
namespace Events {

//! State shared between threads of the network event coupling.
/*!
 *  The class is thread-safe (i.e. all public methods).
 */
class NetworkEventCoupling::SharedState : public virtual Core::CompletionHandlerInterface,
                                          public virtual Core::Timers::HandlerInterface {
  public:
    SharedState(const std::shared_ptr<Core::CompletionHandlerInterface> &completionHandler,
                int completionId, const std::shared_ptr<EventHub> &hub, int hubClientId);
    void RegisterTcpConnection(IO::TcpConnection *connection);
    void ShutDown();
    void OnKeepAlive();
    void OnCompletion(int completionId) override;
    bool OnTimer(int timer) override;

  private:
    std::mutex                                        lock_;
    std::condition_variable                           stateChanged_;
    std::shared_ptr<Core::CompletionHandlerInterface> completionHandler_;
    int                                               completionId_;
    bool                                              keepAlive_;
    bool                                              writerFinished_;
    std::shared_ptr<EventHub>                         hub_;
    std::optional<int>                                hubClientId_;
    IO::TcpConnection                                 *tcpConnection_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_SHAREDSTATE_H_
