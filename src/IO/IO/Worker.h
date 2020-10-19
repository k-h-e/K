#ifndef K_EVENTS_IO_WORKER_H_
#define K_EVENTS_IO_WORKER_H_

#include <K/Core/ActionInterface.h>
#include <K/IO/IO.h>

namespace K {
namespace IO {

//! Worker for the central I/O mechanism.
class IO::Worker : public virtual K::Core::ActionInterface {
  public:
    Worker(std::shared_ptr<SharedState> sharedState);
    void ExecuteAction();

  private:
    std::shared_ptr<SharedState>         sharedState_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_EVENTS_IO_WORKER_H_
