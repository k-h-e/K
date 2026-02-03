/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/IoBuffers.h>

#include <cassert>
#include <mutex>

#include <K/Core/Log.h>

#include "IoBuffer.h"

using std::make_shared;
using std::make_unique;
using std::mutex;
using std::to_string;
using std::unique_lock;

using K::Core::ByteSpanInterface;
using K::Core::Log;
using K::Core::UniqueHandle;

namespace K {
namespace IO {

IoBuffers::IoBuffers()
        : state_{make_shared<State>()} {
    state_->groups.push_back(make_unique<Group>(  16, 16, *state_));
    state_->groups.push_back(make_unique<Group>(  32, 16, *state_));
    state_->groups.push_back(make_unique<Group>(  64, 16, *state_));
    state_->groups.push_back(make_unique<Group>( 128,  8, *state_));
    state_->groups.push_back(make_unique<Group>( 256,  8, *state_));
    state_->groups.push_back(make_unique<Group>( 512,  8, *state_));
    state_->groups.push_back(make_unique<Group>(1024,  4, *state_));
    state_->groups.push_back(make_unique<Group>(2048,  4, *state_));
    state_->groups.push_back(make_unique<Group>(4096,  2, *state_));
}

UniqueHandle<ByteSpanInterface> IoBuffers::Get(int size) {
    assert(size > 0);
    Group::IoBuffer *buffer;
    {
        unique_lock<mutex> critical(state_->mutex);    // ..............................................................
        Group &group { SelectGroup(size) };
        buffer = &group.Get();
        buffer->SetSize(size <= group.BufferSize() ? size : group.BufferSize());
    }    // ............................................................................................................

    return UniqueHandle<ByteSpanInterface>{*buffer, *buffer, state_};
}

void IoBuffers::LogStatistics() {
    unique_lock<mutex> critical(state_->mutex);    // ..................................................................
    Log::Print(Log::Level::Info, this, [&]{
        return "buffers_in_use=" + to_string(state_->numBuffersInUse) + "/" + to_string(state_->numBuffersTotal)
            + ", bytes_in_use=" + to_string(state_->numBytesInUse) + "/"  + to_string(state_->numBytesTotal);
    });
    for (auto &group : state_->groups) {
        group->LogStatistics();
    }
}    // ................................................................................................................

// ---

IoBuffers::Group &IoBuffers::SelectGroup(int size) {
    for (auto &group : state_->groups) {
        if (group->BufferSize() >= size) {
            return *group;
        }
    }

    return *(state_->groups.back());
}

}    // Namespace IO.
}    // Namespace K.
