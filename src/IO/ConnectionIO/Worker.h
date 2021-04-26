#ifndef K_IO_CONNECTIONIO_WORKER_H_
#define K_IO_CONNECTIONIO_WORKER_H_

#include <sys/select.h>
#include <unordered_map>
#include <unordered_set>
#include <K/Core/ActionInterface.h>
#include <K/IO/ConnectionIO.h>
#include "WorkInfo.h"

namespace K {
namespace IO {

//! Worker for the central I/O mechanism.
class ConnectionIO::Worker : public virtual K::Core::ActionInterface {
  public:
    Worker(int pipe, std::shared_ptr<SharedState> sharedState);
    Worker(const Worker &other)            = delete;
    Worker &operator=(const Worker &other) = delete;
    Worker(Worker &&other)                 = delete;
    Worker &operator=(Worker &&other)      = delete;
    ~Worker();
    void ExecuteAction();

  private:
    static const int bufferSize = 8192;

    struct ClientInfo {
        int                                            fileDescriptor;
        std::shared_ptr<ConnectionIO::ClientInterface> client;
        bool                                           canRead;
        bool                                           canWrite;
        bool                                           eof;
        bool                                           error;
        bool                                           unregistering;

        ClientInfo()
            : fileDescriptor(-1),
              canRead(false),
              canWrite(false),
              eof(false),
              error(false),
              unregistering(false) {
            // Nop.
        }

        ClientInfo(int aFileDescriptor, const std::shared_ptr<ConnectionIO::ClientInterface> &aClient)
            : fileDescriptor(aFileDescriptor),
              client(aClient),
              canRead(false),
              canWrite(false),
              eof(false),
              error(false),
              unregistering(false) {
            // Nop.
        }
        // Defaut copy/move, okay.
    };

    bool BlockSigPipe();
    void SetUpSelectSets();
    void UpdateHighestFileDescriptor(int fileDescriptor);
    void DoIO();
    void UnregisterClients();
    bool ProcessClientRequests();
    void Read(ClientInfo *clientInfo);
    void Write(ClientInfo *clientInfo);
    void SetClientError(ClientInfo *clientInfo);
    void ScheduleClientDeregistration(const ClientInfo &clientInfo);

    std::shared_ptr<SharedState> sharedState_;

    int                                               pipe_;
    fd_set                                            readSet_;
    fd_set                                            writeSet_;
    int                                               highestFileDescriptor_;
    uint8_t                                           buffer_[bufferSize];
    WorkInfo                                          workInfo_;
    std::unordered_map<ClientInterface *, ClientInfo> clients_;
    std::vector<ClientInterface *>                    clientsToUnregister_;
    std::unordered_set<int>                           fileDescriptors_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONNECTIONIO_WORKER_H_
