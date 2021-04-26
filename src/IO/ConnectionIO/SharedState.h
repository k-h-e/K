#ifndef K_IO_CONNECTIONIO_SHAREDSTATE_H_
#define K_IO_CONNECTIONIO_SHAREDSTATE_H_

#include <vector>
#include <mutex>
#include <condition_variable>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/IO/ConnectionIO.h>
#include "WorkInfo.h"

namespace K {
namespace IO {

struct WorkInfo;

//! State shared between threads of the central I/O mechanism.
/*!
 *  The class is thread-safe (i.e. all public methods).
 */
class ConnectionIO::SharedState : public virtual K::Core::CompletionHandlerInterface {
  public:
    SharedState(int pipe);
    SharedState(const SharedState &other)            = delete;
    SharedState &operator=(const SharedState &other) = delete;
    SharedState(SharedState &&other)                 = delete;
    SharedState &operator=(SharedState &&other)      = delete;
    ~SharedState();

    bool Register(const std::shared_ptr<ClientInterface> &client, int fd);
    void Unregister(const std::shared_ptr<ClientInterface> &client, bool *outFinalStreamError);
    void SetClientCanRead(ClientInterface *client);
    void SetClientCanWrite(ClientInterface *client);
    void RequestCustomCall(ClientInterface *client);
    void ShutDown();

    // Called from worker...
    void GetWork(WorkInfo *outInfo);
    void OnErrorState();
    void OnClientRegistered(bool success);
    void OnClientUnregistered(bool finalStreamError);
    void OnCompletion(int completionId) override;

  private:
    void NotifyWorker();
    void DoShutDown(std::unique_lock<std::mutex> *critical);

    std::mutex                     lock_;
    std::condition_variable        stateChanged_;
    int                            pipe_;
    bool                           registrationOperationRunning_;
    RegistrationInfo               registrationInfo_;
    UnregistrationInfo             unregistrationInfo_;
    std::vector<ClientInterface *> clientsReadyToRead_;
    std::vector<ClientInterface *> clientsReadyToWrite_;
    std::vector<ClientInterface *> clientsWithCustomCallRequested_;
    bool                           error_;
    bool                           shutDownRequested_;
    bool                           workerFinished_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONNECTIONIO_SHAREDSTATE_H_
