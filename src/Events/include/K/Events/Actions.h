/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_ACTIONS_H_
#define K_EVENTS_ACTIONS_H_

#include <K/Core/ReusableItems.h>
#include <K/Core/ActionInterface.h>

namespace K {
namespace Events {

//! Keeps track of actions to perform.
class Actions {
  public:
    Actions() : actions_(1) {};
    Actions(const Actions &other)            = delete;
    Actions &operator=(const Actions &other) = delete;
    Actions(Actions &&other)                 = delete;
    Actions &operator=(Actions &&other)      = delete;
    //! Registers the specified action as weak reference (dumb pointer), and returns a non-negative id for unregistering
    //! it later.
    int Register(Core::ActionInterface *action);
    //! Unregisters the specified action.
    void Unregister(int id);
    //! Executes the registered actions, once each.
    void Execute();
    
  private:
    Core::ReusableItems<Core::ActionInterface *> actions_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_ACTIONS_H_
