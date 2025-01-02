/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Events/NetworkEventCoupling.h>

#include <cstring>

#include <K/Core/IoBufferInterface.h>
#include <K/Core/Log.h>
#include <K/Core/StringTools.h>
#include <K/Core/Timer.h>
#include <K/IO/ConnectionEndPoint.h>
#include <K/IO/KeepAliveParameters.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventHub.h>
#include <K/Events/EventNotifier.h>

using std::make_unique;
using std::move;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;
using K::Core::Buffer;
using K::Core::IoBufferInterface;
using K::Core::IoBuffers;
using K::Core::Log;
using K::Core::RunLoop;
using K::Core::StreamInterface;
using K::Core::StringTools;
using K::Core::Timers;
using K::Core::Timer;
using K::Core::UniqueHandle;
using K::IO::ConnectionEndPoint;
using K::IO::KeepAliveParameters;
using K::IO::TcpConnection;

namespace K {
namespace Events {

NetworkEventCoupling::NetworkEventCoupling(
    const shared_ptr<TcpConnection> &tcpConnection, const string &protocolVersion,
    const KeepAliveParameters &keepAliveParameters, const shared_ptr<EventHub> &hub,
    const shared_ptr<Event> &connectedEvent, const shared_ptr<Event> &disconnectedEvent, 
    const shared_ptr<RunLoop> &runLoop, const shared_ptr<IoBuffers> &ioBuffers, const shared_ptr<Timers> &timers)
        : hub_{hub},
          tcpConnection_{make_unique<ConnectionEndPoint>(tcpConnection, runLoop, ioBuffers)},
          runLoop_{runLoop},
          protocolVersion_{protocolVersion},
          handler_{nullptr},
          connectedEvent_{connectedEvent},
          disconnectedEvent_{disconnectedEvent},
          state_{State::AcceptingChunkSize},
          readCursor_{0},
          readChunkSize_{0},
          protocolVersionMatch_{false},
          keepAliveReceived_{false},
          error_{false},
          signalErrorState_{false},
          disconnectedEventPosted_{false} {
    hubClientId_ = hub_->RegisterEventLoop();
    tcpConnection_->Register(this);

    numSendsBetweenKeepAliveChecks_ = static_cast<int>(keepAliveParameters.CheckInterval().count())
                                          / static_cast<int>(keepAliveParameters.SendInterval().count());
    if (numSendsBetweenKeepAliveChecks_ * static_cast<int>(keepAliveParameters.SendInterval().count())
            < static_cast<int>(keepAliveParameters.CheckInterval().count())) {
        ++numSendsBetweenKeepAliveChecks_;
    }
    numKeepAliveSendsUntilCheck_ = numSendsBetweenKeepAliveChecks_;

    timer_ = make_unique<Timer>(keepAliveParameters.SendInterval(), runLoop_, timers);
    timer_->Register(this);

    eventBuffer_   = make_unique<Buffer>();
    eventNotifier_ = make_unique<EventNotifier>(hub_, hubClientId_, runLoop_);
    eventNotifier_->Register(this);

    runLoopClientId_ = runLoop_->AddClient(this);

    SendVersionChunk();
}

NetworkEventCoupling::~NetworkEventCoupling() {
    EnsureDisconnectedEventPosted();

    runLoop_->RemoveClient(runLoopClientId_);
    eventNotifier_.reset();
    timer_.reset();
    tcpConnection_.reset();
    hub_->UnregisterEventLoop(hubClientId_);

    // TODO: Check deconstruction.
}

void NetworkEventCoupling::Register(NetworkEventCoupling::HandlerInterface *handler) {
    if (handler) {
        handler_ = handler;
        if (error_) {
            signalErrorState_ = true;
            runLoop_->RequestActivation(runLoopClientId_, false);
        }
    } else {
        handler_ = nullptr;;
    }
}

bool NetworkEventCoupling::ErrorState() const {
    return error_;
}

void NetworkEventCoupling::OnRawStreamData(UniqueHandle<IoBufferInterface> buffer) {
    readBuffer_.AppendFromMemory(buffer->Content(), buffer->Size());
    uint8_t *bufferU8 = static_cast<uint8_t *>(readBuffer_.Data());

    uint32_t  chunkSizeU32;
    const int sizeFieldSize = static_cast<int>(sizeof(chunkSizeU32));
    bool      done          = false;
    while (!done && !error_) {
        int numToProcess = readBuffer_.DataSize() - readCursor_;
        switch (state_) {
            case State::AcceptingChunkSize:
                if (numToProcess >= sizeFieldSize) {
                    memcpy(&chunkSizeU32, &bufferU8[readCursor_], sizeFieldSize);
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
                    memcpy(&chunkType, &bufferU8[readCursor_], sizeof(chunkType));
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
                                        hub_->Submit(hubClientId_, &bufferU8[readCursor_ + offset], eventDataSize,
                                                     true);
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
                                    if (StringTools::Deserialize(&remoteProtocolVersion,
                                                                 &bufferU8[readCursor_ + offset], length)) {
                                        if (protocolVersion_ == remoteProtocolVersion) {
                                            protocolVersionMatch_ = true;
                                            if (connectedEvent_) {
                                                hub_->Post(hubClientId_, *connectedEvent_, true);
                                            }
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

void NetworkEventCoupling::OnStreamError(StreamInterface::Error error) {
    (void) error;
    EnterErrorState();
}

void NetworkEventCoupling::OnTimer() {
    if (!error_) {    // Defensive.
        SendKeepAliveChunk();
        if (!error_) {
            --numKeepAliveSendsUntilCheck_;
            if (numKeepAliveSendsUntilCheck_ <= 0) {
                if (keepAliveReceived_) {
                    keepAliveReceived_           = false;
                    numKeepAliveSendsUntilCheck_ = numSendsBetweenKeepAliveChecks_;
                    //Log::Print(Log::Level::Debug, this, [&]{ return "keep-alive check OK"; });
                } else {
                    Log::Print(Log::Level::Error, this, [&]{ return "keep-alive not received in time"; });
                    EnterErrorState();
                }
            }
        }
    }
}

void NetworkEventCoupling::OnEventsAvailable() {
    if (!error_) {    // Defensive.
        eventBuffer_->Clear();
        if (hub_->Sync(hubClientId_, &eventBuffer_)) {
            if (eventBuffer_->DataSize()) {
                SendEventsChunk(eventBuffer_->Data(), eventBuffer_->DataSize());
            }
        } else {
            EnterErrorState();
        }
    }
}

void NetworkEventCoupling::Activate(bool deepActivation) {
    (void)deepActivation;
    if (signalErrorState_) {
        signalErrorState_ = false;
        if (handler_) {
            handler_->OnNetworkEventCouplingErrorState();
        }
    }
}

void NetworkEventCoupling::CopyDown() {
    if (readCursor_ >= 4096) {
        int numRemaining = readBuffer_.DataSize() - readCursor_;
        if (numRemaining) {
            std::memcpy(readBuffer_.Data(), &static_cast<uint8_t *>(readBuffer_.Data())[readCursor_], numRemaining);
        }
        readBuffer_.Shrink(numRemaining);
        readCursor_ = 0;
    }
}

void NetworkEventCoupling::SendVersionChunk() {
    vector<uint8_t> versionBinary;
    StringTools::Serialize(protocolVersion_, &versionBinary);

    ChunkType chunkType = ChunkType::Version;
    uint32_t  chunkSize = static_cast<uint32_t>(sizeof(chunkType) + versionBinary.size());
    (*tcpConnection_) << chunkSize;
    (*tcpConnection_) << chunkType;
    WriteItem(tcpConnection_.get(), &versionBinary[0], static_cast<int>(versionBinary.size()));
}

void NetworkEventCoupling::SendEventsChunk(const void *data, int dataSize) {
    ChunkType chunkType = ChunkType::Events;
    uint32_t  chunkSize = static_cast<uint32_t>(dataSize) + static_cast<uint32_t>(sizeof(chunkType));
    (*tcpConnection_) << chunkSize;
    (*tcpConnection_) << chunkType;
    WriteItem(tcpConnection_.get(), data, dataSize);
}

void NetworkEventCoupling::SendKeepAliveChunk() {
    ChunkType chunkType = ChunkType::KeepAlive;
    uint32_t  chunkSize = static_cast<uint32_t>(sizeof(chunkType));
    (*tcpConnection_) << chunkSize;
    (*tcpConnection_) << chunkType;
}

void NetworkEventCoupling::EnterErrorState() {
    if (!error_) {
        EnsureDisconnectedEventPosted();
        
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

void NetworkEventCoupling::EnsureDisconnectedEventPosted() {
    if (!disconnectedEventPosted_) {
        if (disconnectedEvent_ && protocolVersionMatch_) {
            hub_->Post(hubClientId_, *disconnectedEvent_, true);
            disconnectedEventPosted_ = true;
        }
    }
}

}    // Namespace Events.
}    // Namespace K.
