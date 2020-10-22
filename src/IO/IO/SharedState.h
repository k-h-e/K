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
    SharedState(int pipe);
    SharedState(const SharedState &other)            = delete;
    SharedState &operator=(const SharedState &other) = delete;
    SharedState(SharedState &&other)                 = delete;
    SharedState &operator=(SharedState &&other)      = delete;
    ~SharedState();
    bool Register(int fd, ReadHandlerInterface *reader, WriteHandlerInterface *writer);
    void Unregister(int fd);
    void SetErrorState();
    void ShutDown();
    void OnCompletion(int completionId) override;

  private:
    void WakeWorker();

    std::mutex              lock_;
    std::condition_variable stateChanged_;
    bool                    error_;
    bool                    shutDownRequested_;
    bool                    workerFinished_;
    int                     pipe_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_IO_SHAREDSTATE_H
