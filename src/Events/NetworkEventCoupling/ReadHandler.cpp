#include "ReadHandler.h"

#include <cstring>
#include <K/Core/Log.h>
#include <K/Core/StringTools.h>
#include <K/Events/EventHub.h>
#include "SharedState.h"

using std::memcpy;
using std::shared_ptr;
using std::string;
using std::to_string;
using K::Core::Log;
using K::Core::StringTools;
using K::Events::EventLoopHub;

namespace K {
namespace Events {

NetworkEventCoupling::ReadHandler::ReadHandler(const string &protocolVersion, const shared_ptr<EventHub> &hub,
                                               int hubClientId, const shared_ptr<SharedState> &sharedState)
        : sharedState_(sharedState),
          hub_(hub),
          hubClientId_(hubClientId),
          protocolVersion_(protocolVersion),
          protocolVersionMatch_(false),
          state_(State::AcceptingChunkSize),
          cursor_(0),
          chunkSize_(0),
          error_(false) {
    // Nop.
}

void NetworkEventCoupling::ReadHandler::HandleStreamData(const void *data, int dataSize) {
    if (!error_) {
        buffer_.Append(data, dataSize);
        uint8_t *buffer = static_cast<uint8_t *>(buffer_.Data());

        uint32_t  chunkSizeU32;
        const int sizeFieldSize = static_cast<int>(sizeof(chunkSizeU32));
        bool      done          = false;
        while (!done && !error_) {
            int numToProcess = buffer_.DataSize() - cursor_;
            switch (state_) {
                case State::AcceptingChunkSize:
                    if (numToProcess >= sizeFieldSize) {
                        memcpy(&chunkSizeU32, &buffer[cursor_], sizeFieldSize);
                        chunkSize_ = static_cast<int>(chunkSizeU32);
                        if (chunkSize_ >= static_cast<int>(sizeof(ChunkType))) {
                            cursor_ += sizeFieldSize;
                            state_   = State::AcceptingChunkData;
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
                    if (numToProcess >= chunkSize_) {
                        ChunkType chunkType;
                        memcpy(&chunkType, &buffer[cursor_], sizeof(chunkType));
                        switch (chunkType) {
                            case ChunkType::KeepAlive:
                                sharedState_->OnKeepAlive();
                                break;
                            case ChunkType::Events:
                                {
                                    if (protocolVersionMatch_) {
                                        int offset = static_cast<int>(sizeof(chunkType));
                                        if (chunkSize_ > offset) {
                                            int eventDataSize = chunkSize_ - offset;
                                            hub_->Submit(hubClientId_, &buffer[cursor_ + offset], eventDataSize, true);
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
                                    if (chunkSize_ > offset) {
                                        int length = chunkSize_ - offset;
                                        string remoteProtocolVersion;
                                        if (StringTools::Deserialize(&remoteProtocolVersion, &buffer[cursor_ + offset],
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

                        cursor_ += chunkSize_;
                        state_   = State::AcceptingChunkSize;
                    }
                    else {
                        CopyDown();
                        done = true;
                    }
                    break;
            }
        }
    }
}

void NetworkEventCoupling::ReadHandler::HandleEof() {
    Log::Print(Log::Level::Warning, this, []{ return "EOF"; });
    EnterErrorState();
}

void NetworkEventCoupling::ReadHandler::HandleError() {
    Log::Print(Log::Level::Warning, this, []{ return "I/O error"; });
    EnterErrorState();
}

void NetworkEventCoupling::ReadHandler::CopyDown() {
    if (cursor_ >= 4096) {
        int numRemaining = buffer_.DataSize() - cursor_;
        if (numRemaining) {
            std::memcpy(buffer_.Data(), &static_cast<uint8_t *>(buffer_.Data())[cursor_], numRemaining);
        }
        buffer_.Shrink(numRemaining);
        cursor_ = 0;
    }
}

void NetworkEventCoupling::ReadHandler::EnterErrorState() {
    if (!error_) {
        hub_->RequestShutDown(hubClientId_);
        error_ = true;

        Log::Print(Log::Level::Warning, this, []{ return "entered error state"; });
    }
}

}    // Namespace Events.
}    // Namespace K.

