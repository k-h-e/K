/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/IoBufferQueue.h>

#include <cstring>

#include <K/Core/IoBufferInterface.h>
#include <K/Core/IoBuffers.h>
#include <K/Core/NonBlockingOutStreamInterface.h>

using std::memcpy;
using std::size_t;

namespace K {
namespace Core {

IoBufferQueue::IoBufferQueue()
        : numBytesTotal_{0u} {
    // Nop.
}

size_t IoBufferQueue::PayloadSize() const {
    return numBytesTotal_;
}

bool IoBufferQueue::Empty() const {
    return buffers_.empty();
}

void IoBufferQueue::Clear() {
    buffers_.clear();
    numBytesTotal_ = 0u;
}

UniqueHandle<IoBufferInterface> IoBufferQueue::Get() {
    UniqueHandle<IoBufferInterface> buffer;
    if (!buffers_.empty()) {
        buffer = std::move(buffers_.front());
        buffers_.pop_front();
        numBytesTotal_ -= static_cast<size_t>(buffer->Size());
    }

    return buffer;
} 

void IoBufferQueue::PutBack(UniqueHandle<IoBufferInterface> buffer) {
    numBytesTotal_ += static_cast<size_t>(buffer->Size());
    buffers_.push_front(std::move(buffer));
}

void IoBufferQueue::Put(UniqueHandle<IoBufferInterface> buffer) {
    numBytesTotal_ += static_cast<size_t>(buffer->Size());
    buffers_.push_back(std::move(buffer));
}

UniqueHandle<IoBufferInterface> IoBufferQueue::GetBack() {
    UniqueHandle<IoBufferInterface> buffer;
    if (!buffers_.empty()) {
        buffer = std::move(buffers_.back());
        buffers_.pop_back();
        numBytesTotal_ -= static_cast<size_t>(buffer->Size());
    }

    return buffer;
}

// ---

void Transfer(IoBufferQueue &source, IoBufferQueue &destination, size_t maxPayloadSize) {
    while (!source.Empty() && (destination.PayloadSize() <= maxPayloadSize)) {
        destination.Put(source.Get());
    }
}

void Transfer(IoBufferQueue &source, NonBlockingOutStreamInterface &stream) {
    bool done{false};
    while (!done) {
        auto buffer = source.Get();
        if (!buffer) {
            done = true;
        } else {
            buffer = stream.WriteNonBlocking(std::move(buffer));
            if (buffer) {
                source.PutBack(std::move(buffer));
                done = true;
            }
        }
    }
}

void Put(const void *sourceData, int sourceDataSize, IoBufferQueue &destination, IoBuffers &ioBuffers) {
    const uint8_t *source { static_cast<const uint8_t *>(sourceData) };
    int           numLeft { sourceDataSize };
    while (numLeft > 0) {
        auto buffer = ioBuffers.Get(numLeft);
        memcpy(buffer->Content(), source, static_cast<size_t>(buffer->Size()));
        source  += buffer->Size();
        numLeft -= buffer->Size();
        destination.Put(std::move(buffer));
    }
}

}    // Namespace Core.
}    // Namespace K.
