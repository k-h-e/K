#ifndef K_EVENTS_NETWORKEVENTCOUPLING_READHANDLER_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_READHANDLER_H_

#include <memory>
#include <K/Core/Buffer.h>
#include <K/IO/StreamHandlerInterface.h>
#include <K/Events/NetworkEventCoupling.h>

namespace K {
namespace Events {

class EventLoopHub;

//! Asynchronous read handler for the network event coupling.
class NetworkEventCoupling::ReadHandler : public virtual IO::StreamHandlerInterface {
  public:
    ReadHandler(const std::shared_ptr<EventLoopHub> &hub, int hubClientId,
                const std::shared_ptr<SharedState> &sharedState);
    void HandleStreamData(const void *data, int dataSize) override;
    void HandleEof() override;
    void HandleError() override;

  private:
    enum class State { AcceptingChunkSize,
                       AcceptingChunkData  };

    void CopyDown();
    void EnterErrorState();

    std::shared_ptr<SharedState>  sharedState_;    // Thread-safe.
    std::shared_ptr<EventLoopHub> hub_;            // Thread-safe.

    int                      hubClientId_;
    uint32_t                 version_;
    State                    state_;
    K::Core::Buffer          buffer_;
    int                      cursor_;
    int                      chunkSize_;
    bool                     error_;
};


}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_READHANDLER_H_
