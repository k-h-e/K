#ifndef K_EVENTS_IO_WORKINFO_H_
#define K_EVENTS_IO_WORKINFO_H_

#include <vector>
#include <K/IO/ConnectionIO.h>

namespace K {
namespace IO {

struct ConnectionIO::RegistrationInfo {
    std::shared_ptr<ConnectionIO::ClientInterface> client;
    int                                            fileDescriptor;
    bool                                           acceptedByWorker;
    bool                                           success;
    bool                                           finished;

    RegistrationInfo()
        : fileDescriptor(-1),
          acceptedByWorker(false),
          success(false),
          finished(false) {
        // Nop.
    }
    RegistrationInfo(const std::shared_ptr<ConnectionIO::ClientInterface> &aClient, int aFileDescriptor)
        : client(aClient),
          fileDescriptor(aFileDescriptor),
          acceptedByWorker(false),
          success(false),
          finished(false) {
        // Nop.
    }
    // Default copy/move, okay.
};

struct ConnectionIO::UnregistrationInfo {
    std::shared_ptr<ConnectionIO::ClientInterface> client;
    bool                                           acceptedByWorker;
    bool                                           finalStreamError;
    bool                                           finished;

    UnregistrationInfo()
        : acceptedByWorker(false),
          finalStreamError(false),
          finished(false) {
        // Nop.
    }
    UnregistrationInfo(const std::shared_ptr<ConnectionIO::ClientInterface> &aClient)
        : client(aClient),
          acceptedByWorker(false),
          finalStreamError(false),
          finished(false) {
        // Nop.
    }
    // Default copy/move, okay.
};

//! Describes work for the I/O worker.
struct ConnectionIO::WorkInfo {
    bool                                         shutDownRequested;
    RegistrationInfo                             registrationInfo;
    UnregistrationInfo                           unregistrationInfo;
    std::vector<ConnectionIO::ClientInterface *> clientsReadyToRead;
    std::vector<ConnectionIO::ClientInterface *> clientsReadyToWrite;
    std::vector<ConnectionIO::ClientInterface *> clientsWithCustomCallRequested;

    WorkInfo() {
        Clear();
    }
    // Default copy/move, okay.

    void Clear() {
        shutDownRequested  = false;
        registrationInfo   = RegistrationInfo();
        unregistrationInfo = UnregistrationInfo();
        clientsReadyToRead.clear();
        clientsReadyToWrite.clear();
        clientsWithCustomCallRequested.clear();
    }
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_EVENTS_IO_WORKINFO_H_
