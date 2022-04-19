#ifndef K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLING_CORE_H_
#define K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLING_CORE_H_

#include <memory>
#include <string>
#include <vector>
#include <K/Core/Buffer.h>
#include <K/Core/ErrorStateInterface.h>
#include <K/Core/RingBuffer.h>
#include <K/Core/Framework/RunLoop.h>
#include <K/Core/Framework/Timer.h>
#include <K/IO/KeepAliveParameters.h>
#include <K/IO/Framework/TcpConnection.h>
#include <K/Events/Framework/EventNotifier.h>
#include <K/Events/Framework/NetworkEventCoupling.h>

namespace K {
    namespace Core {
        class Timers;
        namespace Framework {
            class RunLoop;
            class Timer;
        }
    }
    namespace Events {
        class EventHub;
    }
}

namespace K {
namespace Events {
namespace Framework {

//! Core of the network event coupling.
class NetworkEventCoupling::Core : public virtual IO::Framework::TcpConnection::HandlerInterface,
                                    public virtual K::Core::Framework::Timer::HandlerInterface,
                                    public virtual EventNotifier::HandlerInterface,
                                    public virtual K::Core::Framework::RunLoop::ClientInterface {
  public:
    Core(std::unique_ptr<IO::Framework::TcpConnection> tcpConnection, const std::string &protocolVersion,
         const IO::KeepAliveParameters &keepAliveParameters, const std::shared_ptr<EventHub> &hub,
         const std::shared_ptr<K::Core::Framework::RunLoop> &runLoop, const std::shared_ptr<K::Core::Timers> &timers);
    Core()                             = delete;
    Core(const Core &other)            = delete;
    Core &operator=(const Core &other) = delete;
    Core(Core &&other)                 = delete;
    Core &operator=(Core &&other)      = delete;
    ~Core();

    void Register(NetworkEventCoupling::HandlerInterface *handler, int id);
    bool ErrorState() const;
    void OnStreamReadyRead(int id) override;
    void OnStreamReadyWrite(int id) override;
    void OnTimer(int id) override;
    void OnEventsAvailable(int id) override;
    void Activate(bool deepActivation) override;

  private:
    enum class ChunkType : uint16_t {
        KeepAlive,
        Events,
        Version
    };
    enum class State { AcceptingChunkSize,
                       AcceptingChunkData
    };

    void ProcessIncoming();
    void CopyDown();
    //! Can enter error state.
    void SendVersionChunk();
    //! Can enter error state.
    void SendEventsChunk(const void *data, int dataSize);
    //! Can enter error state.
    void SendKeepAliveChunk();
    //! Can enter error state.
    void PushOut();
    void EnterErrorState();

    const std::shared_ptr<EventHub>                    hub_;                // Thread safe.

    std::unique_ptr<IO::Framework::TcpConnection>      tcpConnection_;      // Present <=> not in error state.
    std::unique_ptr<K::Core::Framework::Timer>         timer_;              // Present <=> not in error state.
    std::unique_ptr<EventNotifier>                     eventNotifier_;      // Present <=> not in error state.
    const std::shared_ptr<K::Core::Framework::RunLoop> runLoop_;
    int                                                runLoopClientId_;
    std::string                                        protocolVersion_;
    int                                                numSendsBetweenKeepAliveChecks_;
    int                                                hubClientId_;
    NetworkEventCoupling::HandlerInterface             *handler_;
    int                                                handlerAssociatedId_;
    State                                              state_;
    K::Core::Buffer                                    readBuffer_;
    int                                                readCursor_;
    int                                                readChunkSize_;
    K::Core::RingBuffer                                writeBuffer_;
    bool                                               canWrite_;
    std::unique_ptr<K::Core::Buffer>                   eventBuffer_;
    bool                                               protocolVersionMatch_;
    int                                                numKeepAliveSendsUntilCheck_;
    bool                                               keepAliveReceived_;
    bool                                               error_;
    bool                                               signalErrorState_;
};

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_FRAMEWORK_NETWORKEVENTCOUPLING_CORE_H_
