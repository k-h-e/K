/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/IoBuffers.h>

#include <cassert>
#include <mutex>

#include "IoBuffer.h"

using std::make_shared;
using std::make_unique;
using std::mutex;
using std::unique_lock;

namespace K {
namespace Core {

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

UniqueHandle<IoBufferInterface> IoBuffers::Get(int size) {
    assert(size > 0);
    Group::IoBuffer *buffer;
    {
        unique_lock<mutex> critical(state_->mutex);    // ..............................................................
        Group &group { SelectGroup(size) };
        buffer = &group.Get();
        buffer->SetSize(size <= group.BufferSize() ? size : group.BufferSize());
    }    // ............................................................................................................

    return UniqueHandle<IoBufferInterface>{*buffer, *buffer, state_};
}

// ---

IoBuffers::Group &IoBuffers::SelectGroup(int size) {
    for (auto &group : state_->groups) {
        if (group->BufferSize() >= size) {
            return *group;
        }
    }

    return *(state_->groups.back());
}

}    // Namespace Core.
}    // Namespace K.
