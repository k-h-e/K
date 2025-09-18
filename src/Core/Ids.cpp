/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/Ids.h>

#include <cassert>

namespace K {
namespace Core {

Ids::Ids()
        : highestId_{0} {
    unusedIds_.insert(0);
}

int Ids::Get() {
    if (!unusedIds_.empty()) {
        int id { *(unusedIds_.begin()) };
        unusedIds_.erase(id);
        return id;
    } else {
        int newHighest { highestId_ + 1 };
        assert (newHighest > 0);
        highestId_ = newHighest;
        return highestId_;
    }
}

void Ids::Put(int id) {
    if ((id >= 0) && (id <= highestId_)) {
        unusedIds_.insert(id);
    }
}

}    // Namespace Core.
}    // Namespace K.
