#include "ReadHandler.h"

#include <cstring>
#include <K/Core/Log.h>
#include <K/Events/EventLoopHub.h>
#include "SharedState.h"

using std::memcpy;
using std::shared_ptr;
using std::to_string;
using K::Core::Log;
using K::Events::EventLoopHub;

namespace K {
namespace Events {

NetworkEventCoupling::ReadHandler::ReadHandler(const shared_ptr<EventLoopHub> &hub, int hubClientId,
                                               const std::shared_ptr<SharedState> &sharedState)
        : sharedState_(sharedState),
          hub_(hub),
          hubClientId_(hubClientId),
          version_(1u),
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
                                    int offset = static_cast<int>(sizeof(chunkType) + sizeof(version_));
                                    if (chunkSize_ > offset) {
                                        int eventDataSize = chunkSize_ - offset;
                                        hub_->Post(hubClientId_, &buffer[cursor_ + offset], eventDataSize, true);
                                    } else {
                                        EnterErrorState();
                                    }
                                }
                                break;
                            default:
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

