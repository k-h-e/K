/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/BufferQueue.h>

#include <cstring>
#include <mutex>

#include <K/Core/BufferProviderInterface.h>
#include <K/Core/ByteSpanInterface.h>
#include <K/Core/NonBlockingOutStreamInterface.h>

using std::memcpy;
using std::size_t;

namespace K {
namespace Core {

BufferQueue::BufferQueue()
        : numBytesTotal_{0u} {
    // Nop.
}

size_t BufferQueue::PayloadSize() const {
    return numBytesTotal_;
}

bool BufferQueue::Empty() const {
    return buffers_.empty();
}

void BufferQueue::Clear() {
    buffers_.clear();
    numBytesTotal_ = 0u;
}

UniqueHandle<ReadableByteSpanInterface> BufferQueue::Get() {
    UniqueHandle<ReadableByteSpanInterface> buffer;
    if (!buffers_.empty()) {
        buffer = std::move(buffers_.front());
        buffers_.pop_front();
        numBytesTotal_ -= static_cast<size_t>(buffer->ByteSpanSize());
    }

    return buffer;
} 

void BufferQueue::PutBack(UniqueHandle<ReadableByteSpanInterface> buffer) {
    numBytesTotal_ += static_cast<size_t>(buffer->ByteSpanSize());
    buffers_.push_front(std::move(buffer));
}

void BufferQueue::Put(UniqueHandle<ReadableByteSpanInterface> buffer) {
    numBytesTotal_ += static_cast<size_t>(buffer->ByteSpanSize());
    buffers_.push_back(std::move(buffer));
}

UniqueHandle<ReadableByteSpanInterface> BufferQueue::GetBack() {
    UniqueHandle<ReadableByteSpanInterface> buffer;
    if (!buffers_.empty()) {
        buffer = std::move(buffers_.back());
        buffers_.pop_back();
        numBytesTotal_ -= static_cast<size_t>(buffer->ByteSpanSize());
    }

    return buffer;
}

// ---

void Transfer(BufferQueue &source, BufferQueue &destination, size_t maxPayloadSize) {
    while (!source.Empty() && (destination.PayloadSize() <= maxPayloadSize)) {
        destination.Put(source.Get());
    }
}

void Transfer(BufferQueue &source, NonBlockingOutStreamInterface &stream) {
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

void Put(const void *sourceData, int sourceDataSize, BufferQueue &destination,
         BufferProviderInterface &bufferProvider) {
    const uint8_t *source { static_cast<const uint8_t *>(sourceData) };
    int           numLeft { sourceDataSize };
    while (numLeft > 0) {
        UniqueHandle<ByteSpanInterface> buffer = bufferProvider.Get(numLeft);
        memcpy(buffer->ByteSpanStart(), source, static_cast<size_t>(buffer->ByteSpanSize()));
        source  += buffer->ByteSpanSize();
        numLeft -= buffer->ByteSpanSize();
        destination.Put(UniqueHandle<ReadableByteSpanInterface>{std::move(buffer)});
    }
}

}    // Namespace Core.
}    // Namespace K.
