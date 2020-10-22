#ifndef K_EVENTS_IO_WORKER_H_
#define K_EVENTS_IO_WORKER_H_

#include <K/Core/ActionInterface.h>
#include <K/IO/IO.h>

namespace K {
namespace IO {

//! Worker for the central I/O mechanism.
class IO::Worker : public virtual K::Core::ActionInterface {
  public:
    Worker(int pipe, std::shared_ptr<SharedState> sharedState);
    Worker(const Worker &other)            = delete;
    Worker &operator=(const Worker &other) = delete;
    Worker(Worker &&other)                 = delete;
    Worker &operator=(Worker &&other)      = delete;
    ~Worker();
    void ExecuteAction();

  private:
    void UpdateHighFD(int fd);

    std::shared_ptr<SharedState>         sharedState_;

    int pipe_;
    int highFD_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_EVENTS_IO_WORKER_H_
