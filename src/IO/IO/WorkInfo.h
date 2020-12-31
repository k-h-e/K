#ifndef K_EVENTS_IO_WORKINFO_H_
#define K_EVENTS_IO_WORKINFO_H_

#include <vector>
#include <K/IO/IO.h>

namespace K {
namespace IO {

struct RegistrationInfo {
    IO::ClientInterface *client;
    int                 fileDescriptor;

    RegistrationInfo()
        : client(nullptr),
          fileDescriptor(-1) {
        // Nop.
    }
    RegistrationInfo(IO::ClientInterface  *aClient, int aFileDescriptor)
        : client(aClient),
          fileDescriptor(aFileDescriptor) {
        // Nop.
    }
    // Default copy/move, okay.
};

//! Describes work for the I/O worker.
struct WorkInfo {
    bool                               shutDownRequested;
    RegistrationInfo                   registrationInfo;
    IO::ClientInterface                *clientToUnregister;
    std::vector<IO::ClientInterface *> clientsReadyToRead;
    std::vector<IO::ClientInterface *> clientsReadyToWrite;

    WorkInfo() {
        Clear();
    }
    // Default copy/move, okay.

    void Clear() {
        shutDownRequested  = false;
        registrationInfo   = RegistrationInfo();
        clientToUnregister = nullptr;
        clientsReadyToRead.clear();
        clientsReadyToWrite.clear();
    }
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_EVENTS_IO_WORKINFO_H_
