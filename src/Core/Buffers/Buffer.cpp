/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "Buffer.h"

#include <cassert>

#include <K/Core/Log.h>
#include <K/Core/StringTools.h>

using std::mutex;
using std::to_string;
using std::unique_lock;

namespace K {
namespace Core {

Buffers::Group::Buffer::Buffer(uint8_t *memory, Buffers::Group *group, int infoId)
        : group_{group},
          infoId_{infoId},
          memory_{memory},
          size_{0} {
    // Nop.
}

Buffers::Group::Buffer::Buffer() : Buffer{nullptr, nullptr, 0} {}

Buffers::Group::Buffer::Buffer(Buffer &&other)
        : group_{nullptr},
          infoId_{0},
          memory_{nullptr},
          size_{0} {
    *this = std::move(other);
}

Buffers::Group::Buffer &Buffers::Group::Buffer::operator=(Buffers::Group::Buffer &&other) {
    group_         = other.group_;
    infoId_        = other.infoId_;
    memory_        = other.memory_;
    size_          = other.size_;

    other.group_         = nullptr;
    other.infoId_        = 0;
    other.memory_        = nullptr;
    other.size_          = 0;

    return *this;
}

void Buffers::Group::Buffer::SetSize(int size) {
    assert(size > 0);
    assert(group_ != nullptr);
    assert(size <= group_->BufferSize());
    size_ = size;

    group_->state_.numBuffersInUse++;
    group_->state_.numBytesInUse += static_cast<uintptr_t>(size_); 

    Log::Print(Log::Level::Debug, this, [&]{
        return "buffer 0x" + StringTools::AddressToHex(memory_) + " now in use, size=" + to_string(size_) + "/"
            + to_string(group_->BufferSize()) + ", buffers_in_use=" + to_string(group_->state_.numBuffersInUse) + "/"
            + to_string(group_->state_.numBuffersTotal) + ", bytes_in_use=" + to_string(group_->state_.numBytesInUse)
            + "/"  + to_string(group_->state_.numBytesTotal);
    });
}

const void *Buffers::Group::Buffer::Data() const {
    assert(memory_ != nullptr);
    return memory_;
}

int Buffers::Group::Buffer::DataSize() const {
    assert(size_ > 0);
    return size_;
}

void Buffers::Group::Buffer::OnReferenceAdded() {
    unique_lock<mutex> critical(group_->state_.mutex);    // ...........................................................
    ++group_->bufferInfos_.Item(infoId_).numReferences;
}    // ................................................................................................................

void Buffers::Group::Buffer::OnReferenceRemoved() {
    unique_lock<mutex> critical(group_->state_.mutex);    // ...........................................................
    BufferInfo &info = group_->bufferInfos_.Item(infoId_);
    assert(info.numReferences > 0);
    --info.numReferences;
    if (info.numReferences == 0) {
        group_->bufferInfos_.Move(infoId_, idleBuffers);
        group_->state_.numBuffersInUse--;
        group_->state_.numBytesInUse -= static_cast<uintptr_t>(size_); 
        Log::Print(Log::Level::Debug, this, [&]{
            return "buffer 0x" + StringTools::AddressToHex(memory_) + " now idle, size="
                + to_string(group_->BufferSize()) + ", buffers_in_use=" + to_string(group_->state_.numBuffersInUse)
                + "/" + to_string(group_->state_.numBuffersTotal) + ", bytes_in_use="
                + to_string(group_->state_.numBytesInUse) + "/"  + to_string(group_->state_.numBytesTotal);
        });
    }
}    // ................................................................................................................

}    // Namespace Core.
}    // Namespace K.
