#ifndef KXM_ZARCH_GAMELOGIC_SAUCER_H_
#define KXM_ZARCH_GAMELOGIC_SAUCER_H_

#include <memory>
#include <kxm/Zarch/GameLogic/Actor.h>

namespace kxm {
namespace Zarch {

class ActorCreationEvent;

namespace GameLogic {

class Data;

//! Represents an enemy saucer inside the game logic.
/*!
 *  \ingroup ZarchGameLogic
 */
class Saucer : public Actor {
  public:
    Saucer();
    Saucer(const Saucer &other)            = delete;
    Saucer &operator=(const Saucer &other) = delete;
    Saucer(Saucer &&other)                 = delete;
    Saucer &operator=(Saucer &&other)      = delete;
    void Handle(const ActorCreationEvent &event);
    void ExecuteAction();
    
  private:
};

}    // Namespace GameLogic.
}    // Namespace Zarch.
}    // Namespace kxm.

#endif    // KXM_ZARCH_GAMELOGIC_SAUCER_H_
