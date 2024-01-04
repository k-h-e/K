/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Events/ActorNaming.h>

#include <cassert>
#include <cstdio>

using namespace std;

namespace K {
namespace Events {

ActorNaming::ActorNaming() {
    // Nop.
}

ActorName ActorNaming::Get() {
    int id;
    if (freeIds_.size()) {
        id = *freeIds_.begin();
        freeIds_.erase(id);
        ++incarnations_[id];
    }
    else {
        id = (int)incarnations_.size();
        assert(id < INT_MAX);
        incarnations_.push_back(0u);
    }
    //printf("handing out id (%d, %u)\n", id, (unsigned int)incarnations_[id]);
    return ActorName(id, incarnations_[id]);
}

void ActorNaming::Put(const ActorName &name) {
    if (!name.IsNone() && (name.Id() < (int)incarnations_.size())) {
        assert(incarnations_[name.Id()] == name.Incarnation());
        freeIds_.insert(name.Id());
    }
}

}    // Namespace Events.
}    // Namespace K.
