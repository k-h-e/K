#ifndef KXM_GAME_NETWORKEVENTCOUPLINGSERVER_WORKER_H_
#define KXM_GAME_NETWORKEVENTCOUPLINGSERVER_WORKER_H_

#include <K/IO/ListenSocket.h>
#include <kxm/Game/NetworkEventCouplingServer.h>

namespace kxm {
namespace Game {

class EventLoopHub;

//! Worker for the network event coupling server.
/*!
 *  \ingroup Game
 */
class NetworkEventCouplingServer::Worker {
  public:
    Worker(int port, const std::shared_ptr<kxm::Game::EventLoopHub> &hub,
           const std::shared_ptr<SharedState> &sharedState);
    void Run();

  private:
    std::shared_ptr<SharedState>             sharedState_;
    std::shared_ptr<kxm::Game::EventLoopHub> hub_;

    std::unique_ptr<K::IO::ListenSocket> listenSocket_;
};

}    // Namespace Game.
}    // Namespace kxm.

#endif    // KXM_GAME_NETWORKEVENTCOUPLINGSERVER_WORKER_H_