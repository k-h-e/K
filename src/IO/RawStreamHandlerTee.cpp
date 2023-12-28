/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/RawStreamHandlerTee.h>

using std::shared_ptr;
using K::Core::RawStreamHandlerInterface;
using K::Core::StreamInterface;

namespace K {
namespace IO {

RawStreamHandlerTee::RawStreamHandlerTee(const shared_ptr<RawStreamHandlerInterface> &streamHandler1,
                                         const shared_ptr<RawStreamHandlerInterface> &streamHandler2)
        : streamHandler1_{streamHandler1},
          streamHandler2_(streamHandler2) {
    // Nop.
}

void RawStreamHandlerTee::OnRawStreamData(const void *data, int dataSize) {
    streamHandler1_->OnRawStreamData(data, dataSize);
    streamHandler2_->OnRawStreamData(data, dataSize);
}

void RawStreamHandlerTee::OnStreamError(StreamInterface::Error error) {
    streamHandler1_->OnStreamError(error);
    streamHandler2_->OnStreamError(error);
}

}    // Namespace IO.
}    // Namespace K.
