#ifndef K_IO_IO_SHAREDSTATE_H_
#define K_IO_IO_SHAREDSTATE_H_

#include <mutex>
#include <condition_variable>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/IO/IO.h>

namespace K {
namespace IO {

//! State shared between threads of the central I/O mechanism.
/*!
 *  The class is thread-safe (i.e. all public methods).
 */
class IO::SharedState : public virtual K::Core::CompletionHandlerInterface {
  public:
    SharedState();
    bool Register(int fd, ReadHandler *reader, WriteHandler *writer);
    void Unregister(int fd);
    void OnCompletion(int completionId) override;

  private:
    std::mutex              lock_;
    std::condition_variable stateChanged_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_IO_SHAREDSTATE_H
