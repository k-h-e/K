/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/RawStreamHandlerTee.h>

#include <cstring>

#include <K/Core/IoBuffers.h>

using std::memcpy;
using std::shared_ptr;
using K::Core::IoBufferInterface;
using K::Core::IoBuffers;
using K::Core::RawStreamHandlerInterface;
using K::Core::StreamInterface;
using K::Core::UniqueHandle;

namespace K {
namespace IO {

RawStreamHandlerTee::RawStreamHandlerTee(
    const shared_ptr<RawStreamHandlerInterface> &streamHandler1,
    const shared_ptr<RawStreamHandlerInterface> &streamHandler2, const shared_ptr<IoBuffers> &ioBuffers)
        : ioBuffers_{ioBuffers},
          streamHandler1_{streamHandler1},
          streamHandler2_{streamHandler2} {
    // Nop.
}

void RawStreamHandlerTee::OnRawStreamData(UniqueHandle<IoBufferInterface> buffer) {
    workingQueue_.Clear();
    Put(buffer->Content(), buffer->Size(), workingQueue_, *ioBuffers_);
    
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
