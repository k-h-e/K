/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/Buffers.h>

#include <cassert>

#include <K/Core/Log.h>

#include "Buffer.h"

using std::to_string;

namespace K {
namespace Core {

Buffers::Group::Group(int bufferSize, int buffersPerBucket, Buffers::State &state)
        : state_{state},
          bufferSize_{bufferSize},
          buffersPerBucket_{buffersPerBucket},
          alignment_{K_ALIGNMENT_BYTES},
          bufferInfos_{2} {
    assert(bufferSize_ > 0);
    assert(buffersPerBucket_ > 0);
    assert(alignment_ > 0);

    spacing_ = bufferSize_;
    int remainder { bufferSize_ % alignment_};
    if (remainder > 0) {
        spacing_ += alignment_ - remainder;
    }
}

Buffers::Group::~Group() {
    for (Buffer *buffers : bufferBuckets_) {
        delete[] buffers;
    }
    bufferBuckets_.clear();

    for (uint8_t *memoryBucket : memoryBuckets_) {
        delete[] memoryBucket;
    }
    memoryBuckets_.clear();
}

int Buffers::Group::BufferSize() const {
    return bufferSize_;
}

Buffers::Group::Buffer *Buffers::Group::Get() {
    auto iterator { bufferInfos_.Iterate(idleBuffers).begin() };
    if (iterator.AtEnd()) {
        AddBucket();
        iterator = bufferInfos_.Iterate(idleBuffers).begin();
    }

    int id { iterator.ItemId() };
    bufferInfos_.Move(id, inUseBuffers);
    
    BufferInfo &info { bufferInfos_.Item(id) };

    return info.buffer;
}

// ---

void Buffers::Group::AddBucket() {
    uint8_t   *memory { new uint8_t[buffersPerBucket_ * spacing_] };
    uintptr_t address { reinterpret_cast<uintptr_t>(memory) };
    int       offset  { 0 };
    
    uintptr_t remainder { address % static_cast<uintptr_t>(alignment_) };
    if (remainder > 0u) {
        delete[] memory;
        memory    = new uint8_t[buffersPerBucket_*spacing_ + alignment_ - 1];
        address   = reinterpret_cast<uintptr_t>(memory);
        remainder = address % static_cast<uintptr_t>(alignment_);
        if (remainder > 0u) {
            offset = alignment_ - static_cast<int>(remainder);
        }
    }

    memoryBuckets_.push_back(memory);

    Buffer *buffers { new Buffer[buffersPerBucket_] };
    for (int i = 0; i < buffersPerBucket_; ++i) {
        int infoId;
        BufferInfo &info { bufferInfos_.Get(idleBuffers, &infoId) };
        buffers[i] = Buffer{memory + offset, this, infoId};
        info.buffer = &buffers[i];
        offset += spacing_;
    }

    bufferBuckets_.push_back(buffers);

    state_.numBuffersTotal += buffersPerBucket_;
    state_.numBytesTotal   += static_cast<uintptr_t>(buffersPerBucket_) * static_cast<uintptr_t>(bufferSize_); 

    Log::Print(Log::Level::Debug, this, [&]{
        return "buffer bucket added, buffer_size=" + to_string(bufferSize_) + ", num_buffers="
            + to_string(buffersPerBucket_);
    });
}

}    // Namespace Core.
}    // Namespace K.
