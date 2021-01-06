#ifndef K_EVENTS_IO_WORKINFO_H_
#define K_EVENTS_IO_WORKINFO_H_

#include <vector>
#include <K/IO/ConnectionIO.h>

namespace K {
namespace IO {

struct RegistrationInfo {
    ConnectionIO::ClientInterface *client;
    int                           fileDescriptor;
    bool                          acceptedByWorker;
    bool                          success;
    bool                          finished;

    RegistrationInfo()
        : client(nullptr),
          fileDescriptor(-1),
          acceptedByWorker(false),
          success(false),
          finished(false) {
        // Nop.
    }
    RegistrationInfo(ConnectionIO::ClientInterface  *aClient, int aFileDescriptor)
        : client(aClient),
          fileDescriptor(aFileDescriptor),
          acceptedByWorker(false),
          success(false),
          finished(false) {
        // Nop.
    }
    // Default copy/move, okay.
};

struct UnregistrationInfo {
    ConnectionIO::ClientInterface *client;
    bool                          acceptedByWorker;
    bool                          finalStreamError;
    bool                          finished;

    UnregistrationInfo()
        : client(nullptr),
          acceptedByWorker(false),
          finalStreamError(false),
          finished(false) {
        // Nop.
    }
    UnregistrationInfo(ConnectionIO::ClientInterface  *aClient)
        : client(aClient),
          acceptedByWorker(false),
          finalStreamError(false),
          finished(false) {
        // Nop.
    }
    // Default copy/move, okay.
};

//! Describes work for the I/O worker.
struct WorkInfo {
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
