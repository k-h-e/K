//
//  Simulation.h
//  kxm
//
//  Created by Kai Hergenröther on 4/7/13.
//
//


#ifndef KXM_ZARCH_SIMULATION_H_
#define KXM_ZARCH_SIMULATION_H_


#include <boost/shared_ptr.hpp>

#include <kxm/Core/ActionInterface.h>
#include <kxm/Game/EventQueue.h>
#include <kxm/Zarch/events.h>


namespace kxm {

namespace Game {
    class ThreadCouplingBuffer;
}

namespace Zarch {

//! Ties together the simulation subsytems, that together run on the dedicated simulation thread (as
//! opposed to the main UI thread).
/*!
 *  \ingroup Zarch
 */
class Simulation : public Core::ActionInterface {
  public:
    Simulation(boost::shared_ptr<Game::ThreadCouplingBuffer> presentationCouplingBuffer,
               int sendToPresentationDirection);
    void ExecuteAction();
    
  private:
    Simulation(const Simulation &other);
    Simulation &operator=(const Simulation &other);
    
    Game::EventQueue<ZarchEvent::EventType>       eventQueue_;
    boost::shared_ptr<Game::ThreadCouplingBuffer> presentationCouplingBuffer_;
    int                                           sendToPresentationDirection_;
};

}
}


#endif    // KXM_ZARCH_SIMULATION_H_
