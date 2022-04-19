#include "Core.h"

#include <K/Core/Log.h>
#include <K/Core/StringTools.h>
#include <K/Core/Framework/Timer.h>
#include <K/IO/Framework/TcpConnection.h>
#include <K/Events/EventHub.h>
#include <K/Events/Framework/EventNotifier.h>

using std::make_unique;
using std::move;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;
using K::Core::Buffer;
using K::Core::Log;
using K::Core::StringTools;
using K::Core::Timers;
using K::Core::Framework::Timer;
using K::Core::Framework::RunLoop;
using K::IO::KeepAliveParameters;
using K::IO::Framework::TcpConnection;

namespace K {
namespace Events {
namespace Framework {

NetworkEventCoupling::Core::Core(
    unique_ptr<TcpConnection> tcpConnection, const string &protocolVersion,
    const KeepAliveParameters &keepAliveParameters, const shared_ptr<EventHub> &hub,
    const shared_ptr<RunLoop> &runLoop, const shared_ptr<Timers> &timers)
        : hub_(hub),
          tcpConnection_(move(tcpConnection)),
          runLoop_(runLoop),
          protocolVersion_(protocolVersion),
          handler_(nullptr),
          handlerAssociatedId_(0),
          state_(State::AcceptingChunkSize),
          readCursor_(0),
          readChunkSize_(0),
          canWrite_(false),
          protocolVersionMatch_(false),
          keepAliveReceived_(false),
          error_(false),
          signalErrorState_(false) {
    hubClientId_ = hub_->RegisterEventLoop();
    tcpConnection_->Register(this, 0);

    numSendsBetweenKeepAliveChecks_ = static_cast<int>(keepAliveParameters.CheckInterval().count())
                                          / static_cast<int>(keepAliveParameters.SendInterval().count());
    if (numSendsBetweenKeepAliveChecks_ * static_cast<int>(keepAliveParameters.SendInterval().count())
            < static_cast<int>(keepAliveParameters.CheckInterval().count())) {
        ++numSendsBetweenKeepAliveChecks_;
    }
    numKeepAliveSendsUntilCheck_ = numSendsBetweenKeepAliveChecks_;

    timer_ = make_unique<Timer>(keepAliveParameters.SendInterval(), runLoop_, timers);
    timer_->Register(this, 0);

    eventBuffer_   = make_unique<Buffer>();
    eventNotifier_ = make_unique<EventNotifier>(hub_, hubClientId_, runLoop_);
    eventNotifier_->Register(this, 0);

    runLoopClientId_ = runLoop_->AddClient(this);

    SendVersionChunk();
}

NetworkEventCoupling::Core::~Core() {
    runLoop_->RemoveClient(runLoopClientId_);
    eventNotifier_.reset();
    timer_.reset();
    tcpConnection_.reset();
    hub_->UnregisterEventLoop(hubClientId_);

    // TODO: Check desconstruction.
}

void NetworkEventCoupling::Core::Register(NetworkEventCoupling::HandlerInterface *handler, int id) {
    if (handler) {
        handler_             = handler;
        handlerAssociatedId_ = id;
        if (error_) {
            signalErrorState_ = true;
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
    } else {
        handler_             = nullptr;;
        handlerAssociatedId_ = 0;
    }
}

bool NetworkEventCoupling::Core::ErrorState() const {
    return error_;
}
void NetworkEventCoupling::Core::OnStreamReadyRead(int id) {
    (void)id;
    bool done = false;
    while (!error_ && !done) {
        int numRead = readBuffer_.AppendFromReader(tcpConnection_.get(), 2 * 1024);
        if (numRead) {
            ProcessIncoming();
        } else {
            if (tcpConnection_->ErrorState() || tcpConnection_->Eof()) {
                EnterErrorState();
            } else {
                done = true;
            }
        }
    }
}

void NetworkEventCoupling::Core::OnStreamReadyWrite(int id) {
    (void)id;
    if (!error_) {    // Defensive.
        canWrite_ = true;
        PushOut();
    }
}

void NetworkEventCoupling::Core::OnTimer(int id) {
    (void)id;
    if (!error_) {    // Defensive.
        SendKeepAliveChunk();
        if (!error_) {
            --numKeepAliveSendsUntilCheck_;
            if (numKeepAliveSendsUntilCheck_ <= 0) {
                if (keepAliveReceived_) {
                    keepAliveReceived_           = false;
                    numKeepAliveSendsUntilCheck_ = numSendsBetweenKeepAliveChecks_;
                    Log::Print(Log::Level::Debug, this, [&]{ return "keep-alive check OK"; });
                } else {
                    Log::Print(Log::Level::Error, this, [&]{ return "keep-alive not received in time"; });
                    EnterErrorState();
                }
            }
        }
    }
}

void NetworkEventCoupling::Core::OnEventsAvailable(int id) {
    (void)id;
    if (!error_) {    // Defensive.
        eventBuffer_->Clear();
        if (hub_->Sync(hubClientId_, &eventBuffer_)) {
            Log::Print(Log::Level::Debug, this, [&]{
                return "events available, size=" + to_string(eventBuffer_->DataSize());
            });
            if (eventBuffer_->DataSize()) {
                SendEventsChunk(eventBuffer_->Data(), eventBuffer_->DataSize());
            }
        } else {
            EnterErrorState();
        }
    }
}

void NetworkEventCoupling::Core::Activate(bool deepActivation) {
    (void)deepActivation;

    if (signalErrorState_) {
        signalErrorState_ = false;
        if (handler_) {
            handler_->OnNetworkEventCouplingErrorState(handlerAssociatedId_);
        }
    }
}

void NetworkEventCoupling::Core::ProcessIncoming() {
    uint8_t *buffer = static_cast<uint8_t *>(readBuffer_.Data());

    uint32_t  chunkSizeU32;
    const int sizeFieldSize = static_cast<int>(sizeof(chunkSizeU32));
    bool      done          = false;
    while (!done && !error_) {
        int numToProcess = readBuffer_.DataSize() - readCursor_;
        switch (state_) {
            case State::AcceptingChunkSize:
                if (numToProcess >= sizeFieldSize) {
                    memcpy(&chunkSizeU32, &buffer[readCursor_], sizeFieldSize);
                    readChunkSize_ = static_cast<int>(chunkSizeU32);
                    if (readChunkSize_ >= static_cast<int>(sizeof(ChunkType))) {
                        readCursor_ += sizeFieldSize;
                        state_       = State::AcceptingChunkData;
                    }
                    else {
                        EnterErrorState();
                    }
                }
                else {
                    CopyDown();
                    done = true;
                }
                break;
            case State::AcceptingChunkData:
                if (numToProcess >= readChunkSize_) {
                    ChunkType chunkType;
                    memcpy(&chunkType, &buffer[readCursor_], sizeof(chunkType));
                    switch (chunkType) {
                        case ChunkType::KeepAlive:
                            keepAliveReceived_ = true;
                            break;
                        case ChunkType::Events:
                            {
                                if (protocolVersionMatch_) {
                                    int offset = static_cast<int>(sizeof(chunkType));
                                    if (readChunkSize_ > offset) {
                                        int eventDataSize = readChunkSize_ - offset;
                                        hub_->Submit(hubClientId_, &buffer[readCursor_ + offset], eventDataSize, true);
                                    } else {
                                        EnterErrorState();
                                    }
                                } else {
                                    Log::Print(Log::Level::Warning, this, [&]{
                                        return "protocol versions not verified to match!";
                                    });
                                    EnterErrorState();
                                }
                            }
                            break;
                        case ChunkType::Version:
                            {
                                int offset = static_cast<int>(sizeof(chunkType));
                                if (readChunkSize_ > offset) {
                                    int length = readChunkSize_ - offset;
                                    string remoteProtocolVersion;
                                    if (StringTools::Deserialize(&remoteProtocolVersion, &buffer[readCursor_ + offset],
                                                                 length)) {
                                        if (protocolVersion_ == remoteProtocolVersion) {
                                            protocolVersionMatch_ = true;
                                            Log::Print(Log::Level::Debug, this, [&]{
                                                return "protocol version check ok (\"" + remoteProtocolVersion
                                                    + "\")";
                                            });
                                        } else {
                                            Log::Print(Log::Level::Warning, this, [&]{
                                                return "protocol version mismatch, local=\""
                                                    + protocolVersion_ + "\", remote=\"" + remoteProtocolVersion
                                                    + "\"";
                                            });
                                            EnterErrorState();
                                        }
                                    } else {
                                        EnterErrorState();
                                    }
                                } else {
                                    EnterErrorState();
                                }
                            }
                            break;
                        default:
                            Log::Print(Log::Level::Warning, this, [&]{
                                return "skipping chunk of unknown type " + to_string(static_cast<int>(chunkType));
                            });
                            break;
                    }

                    readCursor_ += readChunkSize_;
                    state_       = State::AcceptingChunkSize;
                }
                else {
                    CopyDown();
                    done = true;
                }
                break;
        }
    }
}

void NetworkEventCoupling::Core::CopyDown() {
    if (readCursor_ >= 4096) {
        int numRemaining = readBuffer_.DataSize() - readCursor_;
        if (numRemaining) {
            std::memcpy(readBuffer_.Data(), &static_cast<uint8_t *>(readBuffer_.Data())[readCursor_], numRemaining);
        }
        readBuffer_.Shrink(numRemaining);
        readCursor_ = 0;
    }
}

void NetworkEventCoupling::Core::SendVersionChunk() {
    vector<uint8_t> versionBinary;
    StringTools::Serialize(protocolVersion_, &versionBinary);

    ChunkType chunkType = ChunkType::Version;
    uint32_t  chunkSize = static_cast<uint32_t>(sizeof(chunkType) + versionBinary.size());
    writeBuffer_.Put(&chunkSize, sizeof(chunkSize));
    writeBuffer_.Put(&chunkType, sizeof(chunkType));
    writeBuffer_.Put(&versionBinary[0], static_cast<int>(versionBinary.size()));
    PushOut();
}

void NetworkEventCoupling::Core::SendEventsChunk(const void *data, int dataSize) {
    ChunkType chunkType = ChunkType::Events;
    uint32_t  chunkSize = static_cast<uint32_t>(dataSize) + static_cast<uint32_t>(sizeof(chunkType));
    writeBuffer_.Put(&chunkSize, sizeof(chunkSize));
    writeBuffer_.Put(&chunkType, sizeof(chunkType));
    writeBuffer_.Put(data, dataSize);
    PushOut();
}

void NetworkEventCoupling::Core::SendKeepAliveChunk() {
    ChunkType chunkType = ChunkType::KeepAlive;
    uint32_t  chunkSize = static_cast<uint32_t>(sizeof(chunkType));
    writeBuffer_.Put(&chunkSize, sizeof(chunkSize));
    writeBuffer_.Put(&chunkType, sizeof(chunkType));
    PushOut();
}

void NetworkEventCoupling::Core::PushOut() {
    if (!error_ && canWrite_) {
        writeBuffer_.TransferTo(tcpConnection_.get());
        if (!writeBuffer_.Empty()) {
            canWrite_ = false;
            if (tcpConnection_->ErrorState()) {
                EnterErrorState();
            }
        }
    }
}

void NetworkEventCoupling::Core::EnterErrorState() {
    if (!error_) {
        eventNotifier_.reset();
        timer_.reset();
        tcpConnection_.reset();
        hub_->RequestShutDown(hubClientId_);
        error_ = true;

        signalErrorState_ = true;
        runLoop_->RequestActivation(runLoopClientId_, false);

        Log::Print(Log::Level::Warning, this, []{ return "entered error state"; });
    }
}

}    // Namespace Framework.
}    // Namespace Events.
}    // Namespace K.
