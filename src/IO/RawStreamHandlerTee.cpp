/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/RawStreamHandlerTee.h>

#include <cstring>

#include <K/Core/ReadableByteSpanInterface.h>

using std::memcpy;
using std::shared_ptr;

using K::Core::BufferProviderInterface;
using K::Core::RawStreamHandlerInterface;
using K::Core::ReadableByteSpanInterface;
using K::Core::StreamInterface;
using K::Core::UniqueHandle;

namespace K {
namespace IO {

RawStreamHandlerTee::RawStreamHandlerTee(
    const shared_ptr<RawStreamHandlerInterface> &streamHandler1,
    const shared_ptr<RawStreamHandlerInterface> &streamHandler2,
    const shared_ptr<BufferProviderInterface> &bufferProvider)
        : streamHandler1_{streamHandler1},
          streamHandler2_{streamHandler2},
          bufferProvider_{bufferProvider} {
    // Nop.
}

void RawStreamHandlerTee::OnRawStreamData(UniqueHandle<ReadableByteSpanInterface> buffer) {
    workingQueue_.Clear();
    Put(buffer->ByteSpanStartReadOnly(), buffer->ByteSpanSize(), workingQueue_, *bufferProvider_);
    
    streamHandler1_->OnRawStreamData(std::move(buffer));
    while (auto copy = workingQueue_.Get()) {
        streamHandler2_->OnRawStreamData(std::move(copy));
    }
}

void RawStreamHandlerTee::OnStreamError(StreamInterface::Error error) {
    streamHandler1_->OnStreamError(error);
    streamHandler2_->OnStreamError(error);
}

}    // Namespace IO.
}    // Namespace K.
