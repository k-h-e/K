/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "IoBuffer.h"

#include <cassert>
#include <mutex>

#include <K/Core/StringTools.h>

using std::mutex;
using std::to_string;
using std::unique_lock;

namespace K {
namespace Core {

IoBuffers::Group::IoBuffer::IoBuffer(uint8_t *memory, IoBuffers::Group *group, int infoId)
        : group_{group},
          infoId_{infoId},
          memory_{memory},
          size_{0} {
    // Nop.
}

IoBuffers::Group::IoBuffer::IoBuffer() : IoBuffer{nullptr, nullptr, 0} {}

IoBuffers::Group::IoBuffer::IoBuffer(IoBuffer &&other)
        : group_{nullptr},
          infoId_{0},
          memory_{nullptr},
          size_{0} {
    *this = std::move(other);
}

IoBuffers::Group::IoBuffer &IoBuffers::Group::IoBuffer::operator=(IoBuffers::Group::IoBuffer &&other) {
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

void IoBuffers::Group::IoBuffer::SetSize(int size) {
    assert(size > 0);
    assert(group_ != nullptr);
    assert(size <= group_->BufferSize());
    size_ = size;

    group_->numBuffersInUse_++;
    group_->numBytesInUse_ += static_cast<uintptr_t>(size_); 

    group_->state_.numBuffersInUse++;
    group_->state_.numBytesInUse += static_cast<uintptr_t>(size_); 
}

void *IoBuffers::Group::IoBuffer::Content()  {
    assert(memory_ != nullptr);
    return memory_;
}

int IoBuffers::Group::IoBuffer::Size() const {
    assert(size_ > 0);
    return size_;
}

void IoBuffers::Group::IoBuffer::OnReferenceAdded() {
    unique_lock<mutex> critical(group_->state_.mutex);    // ...........................................................
    ++group_->bufferInfos_.Item(infoId_).numReferences;
}    // ................................................................................................................

void IoBuffers::Group::IoBuffer::OnReferenceRemoved() {
    unique_lock<mutex> critical(group_->state_.mutex);    // ...........................................................
    BufferInfo &info = group_->bufferInfos_.Item(infoId_);
    assert(info.numReferences > 0);
    --info.numReferences;
    if (info.numReferences == 0) {
        group_->bufferInfos_.Move(infoId_, idleBuffers);
        group_->numBuffersInUse_--;
        group_->numBytesInUse_ -= static_cast<uintptr_t>(size_); 
        group_->state_.numBuffersInUse--;
        group_->state_.numBytesInUse -= static_cast<uintptr_t>(size_); 
    }
}    // ................................................................................................................

}    // Namespace Core.
}    // Namespace K.
