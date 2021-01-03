#include <K/Events/NetworkEventCoupling.h>

#include <cstring>
#include <K/Core/ThreadPool.h>
#include <K/Core/Buffer.h>
#include <K/Core/Log.h>
#include <K/IO/AsyncReadInterface.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventLoopHub.h>
#include "SharedState.h"
#include "Writer.h"

using std::shared_ptr;
using std::make_shared;
using std::to_string;
using K::Core::CompletionHandlerInterface;
using K::Core::ThreadPool;
using K::Core::Buffer;
using K::Core::Log;
using K::IO::TcpConnection;

namespace K {
namespace Events {

class NetworkEventCoupling::ReadHandler : public virtual K::IO::AsyncReadInterface::HandlerInterface {
  public:
    ReadHandler(const shared_ptr<EventLoopHub> &hub, int hubClientId);
    void OnDataRead(const void *data, int dataSize) override;
    void OnEof() override;
    void OnError() override;

  private:
    enum class State { WaitingForDataSize,
                       WaitingForData      };

    shared_ptr<EventLoopHub> hub_;

    int                      hubClientId_;
    State                    state_;
    K::Core::Buffer          buffer_;
    int                      dataSize_;
    int                      numTotal_;
};

NetworkEventCoupling::NetworkEventCoupling(
    const shared_ptr<TcpConnection> &tcpConnection, const shared_ptr<EventLoopHub> &hub,
    const shared_ptr<CompletionHandlerInterface> &completionHandler, int completionId,
    const std::shared_ptr<K::Core::ThreadPool> &threadPool)
        : hub_(hub),
          tcpConnection_(tcpConnection) {
    sharedState_ = make_shared<SharedState>(completionHandler, completionId);

    hubClientId_ = hub_->RegisterEventLoop();

    readHandler_ = make_shared<ReadHandler>(hub_, hubClientId_);
    tcpConnection_->Register(readHandler_.get());

    writer_ = make_shared<Writer>(tcpConnection_, hub_, hubClientId_, sharedState_);
    threadPool->Run(writer_, sharedState_, writerCompletionId);    

    Log::Print(Log::Level::Debug, this, []{ return std::string("installed"); });
}

NetworkEventCoupling::~NetworkEventCoupling() {
    Log::Print(Log::Level::Debug, this, []{ return "shutting down..."; });

    hub_->RequestShutDown(hubClientId_);
    sharedState_->WaitForThreadsFinished();

    tcpConnection_->Register(nullptr);

    hub_->UnregisterEventLoop(hubClientId_);
}

NetworkEventCoupling::ReadHandler::ReadHandler(const shared_ptr<EventLoopHub> &hub, int hubClientId)
        : hub_(hub),
          hubClientId_(hubClientId),
          state_(State::WaitingForDataSize),
          dataSize_(0),
          numTotal_(0) {
    // Nop.
}

void NetworkEventCoupling::ReadHandler::OnDataRead(const void *data, int dataSize) {
    buffer_.Append(data, dataSize);
    while (true) {
        switch (state_) {
            case State::WaitingForDataSize:
                if (buffer_.DataSize() >= static_cast<int>(sizeof(uint32_t))) {
                    uint32_t dataSize;
                    std::memcpy(&dataSize, buffer_.Data(), sizeof(uint32_t));
                    dataSize_ = static_cast<int>(dataSize);
                    numTotal_ = static_cast<int>(sizeof(uint32_t)) + dataSize_;
                    state_    = State::WaitingForData;
                }
                else {
                    return;
                }
                break;
            case State::WaitingForData:
                if (buffer_.DataSize() >= numTotal_) {
                    hub_->Post(hubClientId_, &static_cast<uint8_t *>(buffer_.Data())[numTotal_ - dataSize_], dataSize_,
                               true);
                    int numRemaining = buffer_.DataSize() - numTotal_;
                    if (numRemaining) {
                        std::memcpy(buffer_.Data(), &static_cast<uint8_t *>(buffer_.Data())[numTotal_], numRemaining);
                    }
                    buffer_.Shrink(numRemaining);
                    state_ = State::WaitingForDataSize;
                }
                else {
                    return;
                }
                break;
        }
    }
}

void NetworkEventCoupling::ReadHandler::OnEof() {
    Log::Print(Log::Level::Debug, this, []{ return "EOF"; });
}

void NetworkEventCoupling::ReadHandler::OnError() {
    Log::Print(Log::Level::Debug, this, []{ return "error!"; });
}

}    // Namespace Events.
}    // Namespace K.

