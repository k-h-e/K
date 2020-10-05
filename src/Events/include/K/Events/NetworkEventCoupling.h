#ifndef K_EVENTS_NETWORKEVENTCOUPLING_H_
#define K_EVENTS_NETWORKEVENTCOUPLING_H_

#include <memory>
#include <K/Core/Interface.h>

namespace K {
namespace Core {
    class CompletionHandlerInterface;
    class ThreadPool;
}
namespace IO {
    class SocketStream;
}
}

namespace K {
namespace Events {

class EventLoopHub;

//! Extends the event mechanism to other nodes across the network.
class NetworkEventCoupling : public virtual K::Core::Interface {
  public:
    //! Well, constructor.
    /*!
     *  \param completionHandler
     *  Optional. If given, it will be called on an arbitrary thread and with the completion id as parameter when the
     *  network event coupling has shut down.
     */
    NetworkEventCoupling(const std::shared_ptr<K::IO::SocketStream> &stream,
                         const std::shared_ptr<EventLoopHub> &hub,
                         const std::shared_ptr<K::Core::CompletionHandlerInterface> &completionHandler,
                         int completionId,
                         const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    ~NetworkEventCoupling();

  private:
    static const int readerCompletionId = 0;
    static const int writerCompletionId = 1;

    class SharedState;
    class Reader;
    class Writer;

    std::shared_ptr<SharedState>         sharedState_;

    std::shared_ptr<K::IO::SocketStream> stream_;
    std::shared_ptr<EventLoopHub>        hub_;

    std::shared_ptr<Reader>              reader_;
    std::shared_ptr<Writer>              writer_;
    int                                  hubClientId_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_NETWORKEVENTCOUPLING_H_
