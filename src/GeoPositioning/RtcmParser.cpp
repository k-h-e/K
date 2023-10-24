/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/GeoPositioning/RtcmParser.h>

#include <cassert>
#include <K/Core/Log.h>
#include <K/GeoPositioning/RtcmMessageHandlerInterface.h>

using std::shared_ptr;
using std::to_string;
using K::Core::Log;
using K::Core::StreamInterface;

namespace K {
namespace GeoPositioning {

RtcmParser::RtcmParser(const shared_ptr<RtcmMessageHandlerInterface> &handler, int handlerActivationId)
        : handler_{handler},
          handlerActivationId_{handlerActivationId},
          state_{State::BetweenMessages},
          payloadSize_{0},
          numSkipped_{0},
          eof_{false},
          error_{false} {
   // Nop.
}

void RtcmParser::OnStreamData(int id, const void *data, int dataSize) {
    (void)id;
    if (!eof_ && !error_) {
        const uint8_t *dataPtr = static_cast<const uint8_t *>(data);
        for (int i = 0; i < dataSize; ++i) {
            uint8_t byte = *dataPtr++;

            switch (state_) {
                case State::AcceptingHeader:
                    message_.AppendToImage(&byte, 1);
                    if (message_.ImageSize() == 3) {
                        payloadSize_ = message_.PayloadSize();
                        state_ = payloadSize_ ? State::AcceptingPayload : State::AcceptingCrc;
                    }
                    break;
                case State::AcceptingPayload:
                    message_.AppendToImage(&byte, 1);
                    if (message_.ImageSize() == payloadSize_ + 3) {
                        state_ = State::AcceptingCrc;
                    }
                    break;
                case State::AcceptingCrc:
                    message_.AppendToImage(&byte, 1);
                    if (message_.ImageSize() == payloadSize_ + 6) {
                        handler_->OnRtcmMessage(handlerActivationId_, message_);
                        message_.Reset();
                        payloadSize_ = 0;
                        state_ = State::BetweenMessages;
                    }
                    break;
                case State::BetweenMessages:
                default:
                    if (byte == 0xd3u) {
                        message_.AppendToImage(&byte, 1);
                        if (numSkipped_) {
                            Log::Print(Log::Level::Warning, this, [&]{
                                return "skipped " + to_string(numSkipped_) + " bytes between messages";
                            });
                            numSkipped_ = 0;
                        }
                        state_ = State::AcceptingHeader;
                    } else {
                        ++numSkipped_;
                    }
                    break;
            }
        }
    }
}

void RtcmParser::OnStreamEnteredErrorState(int id, StreamInterface::Error error) {
    (void)id;
    if (error == StreamInterface::Error::Eof) {
        if (!eof_) {
            handler_->OnEof(handlerActivationId_);
            eof_ = true;
        }
    } else {
        if (!error_) {
            handler_->OnError(handlerActivationId_);
            error_ = true;
        }
    }
}

}    // Namespace GeoPositioning.
}    // Namespace K.
