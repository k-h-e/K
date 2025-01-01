/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/Buffers.h>

#include <cassert>

#include "Buffer.h"

using std::make_shared;
using std::make_unique;
using std::mutex;
using std::unique_lock;

namespace K {
namespace Core {

Buffers::Buffers()
        : state_{make_shared<State>()} {
    state_->groups.push_back(make_unique<Group>(  32, 16, *state_));
    state_->groups.push_back(make_unique<Group>(  64, 16, *state_));
    state_->groups.push_back(make_unique<Group>( 128,  8, *state_));
    state_->groups.push_back(make_unique<Group>( 256,  8, *state_));
    state_->groups.push_back(make_unique<Group>( 512,  8, *state_));
    state_->groups.push_back(make_unique<Group>(1024,  4, *state_));
    state_->groups.push_back(make_unique<Group>(2048,  4, *state_));
    state_->groups.push_back(make_unique<Group>(4096,  4, *state_));
}

UniqueHandle<BufferInterface> Buffers::Get(int size) {
    assert(size > 0);
    Group::Buffer *buffer;
    {
        unique_lock<mutex> critical(state_->mutex);    // ..............................................................
        Group *group   { SelectGroup(size) };
        int bufferSize { size <= group->BufferSize() ? size : group->BufferSize() };
        buffer = group->Get();
        buffer->SetSize(bufferSize);
    }    // ............................................................................................................

    return UniqueHandle<BufferInterface>{buffer, buffer, state_};
}

// ---

Buffers::Group *Buffers::SelectGroup(int size) {
    for (auto &group : state_->groups) {
        if (group->BufferSize() >= size) {
            return group.get();
        }
    }

    return state_->groups.back().get();
}

}    // Namespace Core.
}    // Namespace K.
