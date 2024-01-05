/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_ACTORNAME_
#define K_EVENTS_ACTORNAME_

#include <cassert>
#include <stdint.h>
#include <climits>

namespace K {
namespace Events {

//! Identifies an actor.
class ActorName {
  public:
    //! Creates a "none" actor name.
    ActorName() : id_(INT_MAX), incarnation_(0u) {}
    //! Creates an actor as specified.
    ActorName(int anId, uint32_t anIncarnation) : id_(anId), incarnation_(anIncarnation) {
        assert((anId >= 0) && (anId < INT_MAX));
    }
    int Id() const { return id_; }
    uint32_t Incarnation() const { return incarnation_; }
    //! Tells whether the name is the "none" name.
    bool IsNone() const { return (id_ == INT_MAX); }
    bool operator==(const ActorName &other) const {
        if (IsNone() && other.IsNone()) {
            return true;
        }
        else {
            return (id_ == other.id_) && (incarnation_ == other.incarnation_);
        }
    }
    
  private:
    int      id_;
    uint32_t incarnation_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_ACTORNAME_
