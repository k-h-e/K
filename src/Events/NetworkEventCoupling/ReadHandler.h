#ifndef K_EVENTS_NETWORKEVENTCOUPLING_READHANDLER_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_READHANDLER_H_

#include <memory>
#include <K/Core/Buffer.h>
#include <K/IO/StreamHandlerInterface.h>
#include <K/Events/NetworkEventCoupling.h>

using std::shared_ptr;

namespace K {
namespace Events {

class EventLoopHub;

//! Asynchronous read handler for the network event coupling.
class NetworkEventCoupling::ReadHandler : public virtual IO::StreamHandlerInterface {
  public:
    ReadHandler(const shared_ptr<EventLoopHub> &hub, int hubClientId);
    void HandleStreamData(const void *data, int dataSize) override;
    void HandleEof() override;
    void HandleError() override;

  private:
    enum class State { AcceptingChunkSize,
                       AcceptingChunkData  };

    void CopyDown();
    void EnterErrorState();

    shared_ptr<EventLoopHub> hub_;

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
