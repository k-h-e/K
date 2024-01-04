/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_ACTORNAMING_
#define K_EVENTS_ACTORNAMING_

#include <vector>
#include <unordered_set>
#include <K/Events/ActorName.h>

namespace K {
namespace Events {

//! Central authority for keeping track of and assigning new actor ids.
class ActorNaming {
  public:
    ActorNaming();
    ActorNaming(const ActorNaming &other)            = delete;
    ActorNaming &operator=(const ActorNaming &other) = delete;
    ActorNaming(ActorNaming &&other)                 = delete;
    ActorNaming &operator=(ActorNaming &&other)      = delete;
    
    //! Creates and hands out a new actor name.
    ActorName Get();
    //! Tells the naming authority that the specified name is no longer needed.
    void Put(const ActorName &name);
    
  private:
    std::vector<uint32_t>   incarnations_;
    std::unordered_set<int> freeIds_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_ACTORNAMING_
