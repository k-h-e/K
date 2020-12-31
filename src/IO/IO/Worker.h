#ifndef K_EVENTS_IO_WORKER_H_
#define K_EVENTS_IO_WORKER_H_

#include <sys/select.h>
#include <unordered_map>
#include <K/Core/ActionInterface.h>
#include <K/IO/IO.h>
#include "WorkInfo.h"

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
    static const int bufferSize = 8192;

    struct ClientInfo {
        int                  fileDescriptor;
        IO::ClientInterface  *client;
        bool                 canRead;
        bool                 canWrite;
        bool                 eof;
        bool                 error;

        ClientInfo()
            : fileDescriptor(-1),
              client(nullptr),
              canRead(false),
              canWrite(false),
              eof(false),
              error(false) {
            // Nop.
        }

        ClientInfo(int aFileDescriptor, IO::ClientInterface *aClient)
            : fileDescriptor(aFileDescriptor),
              client(aClient),
              canRead(false),
              canWrite(false),
              eof(false),
              error(false) {
            // Nop.
        }
        // Defaut copy/move, okay.
    };

    void SetUpSelectSets();
    void UpdateHighestFileDescriptor(int fileDescriptor);
    void doIO();
    bool ProcessClientRequests();
    void Read(ClientInfo *clientInfo);
    void Write(ClientInfo *clientInfo);

    std::shared_ptr<SharedState> sharedState_;

    int                                               pipe_;
    fd_set                                            readSet_;
    fd_set                                            writeSet_;
    int                                               highestFileDescriptor_;
    uint8_t                                           buffer_[bufferSize];
    WorkInfo                                          workInfo_;
    std::unordered_map<ClientInterface *, ClientInfo> clients_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_EVENTS_IO_WORKER_H_
