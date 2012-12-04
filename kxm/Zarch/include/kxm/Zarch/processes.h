//
//  processes.h
//  kxm
//
//  Created by Kai Hergenröther on 12/1/12.
//
//


#ifndef KXM_ZARCH_PROCESSES_H_
#define KXM_ZARCH_PROCESSES_H_


#include <kxm/Game/Process.h>
#include <kxm/Zarch/events.h>

namespace kxm {

namespace Game {
    template<class T> class EventQueue;
    template<class T> class Processes;
}

namespace Zarch {

class ZarchProcess : public Game::Process {
  public:
    enum ProcessType { SeederProcess };
    struct Context : public Process::Context {
        Game::Processes<ProcessType>            *processes;     // Weak reference.
        Game::EventQueue<ZarchEvent::EventType> *eventQueue;    // Weak reference.
    };
};

class SeederProcess : public ZarchProcess {
};

}    // Namespace Zarch.
}    // Namespace kxm.


#endif
