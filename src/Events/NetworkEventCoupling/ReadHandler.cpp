#include "ReadHandler.h"

#include <cstring>
#include <K/Core/Log.h>
#include <K/Events/EventLoopHub.h>

using std::to_string;
using K::Core::Log;
using K::Events::EventLoopHub;

namespace K {
namespace Events {

NetworkEventCoupling::ReadHandler::ReadHandler(const shared_ptr<EventLoopHub> &hub, int hubClientId)
        : hub_(hub),
          hubClientId_(hubClientId),
          state_(State::WaitingForDataSize),
          cursor_(0),
          eventDataSize_(0),
          error_(false) {
    // Nop.
}

void NetworkEventCoupling::ReadHandler::HandleStreamData(const void *data, int dataSize) {
    if (!error_) {
        buffer_.Append(data, dataSize);
        uint8_t *buffer = static_cast<uint8_t *>(buffer_.Data());

        uint32_t  eventDataSizeU32;
        const int sizeFieldSize = static_cast<int>(sizeof(eventDataSizeU32));
        bool      done          = false;
        while (!done && !error_) {
            int numToProcess = buffer_.DataSize() - cursor_;
            switch (state_) {
                case State::WaitingForDataSize:
                    if (numToProcess >= sizeFieldSize) {
                        std::memcpy(&eventDataSizeU32, &buffer[cursor_], sizeFieldSize);
                        eventDataSize_  = static_cast<int>(eventDataSizeU32);
                        if (eventDataSize_ > 0) {
                            cursor_        += sizeFieldSize;
                            state_          = State::WaitingForData;
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
                case State::WaitingForData:
                    if (numToProcess >= eventDataSize_) {
                        hub_->Post(hubClientId_, &buffer[cursor_], eventDataSize_, true);
                        cursor_ += eventDataSize_;
                        state_   = State::WaitingForDataSize;
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

