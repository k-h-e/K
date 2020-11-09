#ifndef K_EVENTS_IO_WORKINFO_H_
#define K_EVENTS_IO_WORKINFO_H_

#include <vector>
#include <K/IO/IO.h>

namespace K {
namespace IO {

struct RegistrationInfo {
    int                 fileDescriptor;
    IO::ClientInterface *client;

    RegistrationInfo()
        : fileDescriptor(-1),
          client(nullptr) {
        // Nop.
    }
    RegistrationInfo(int aFileDescriptor, IO::ClientInterface  *aClient)
        : fileDescriptor(aFileDescriptor),
          client(aClient) {
        // Nop.
    }
    // Default copy/move, okay.
};

//! Describes work for the I/O worker.
struct WorkInfo {
    bool             shutDownRequested;
    RegistrationInfo registrationInfo;
    int              fileDescriptorToUnregister;
    std::vector<int> clientsReadyToRead;
    std::vector<int> clientsReadyToWrite;

    WorkInfo() {
        Clear();
    }
    // Default copy/move, okay.

    void Clear() {
        shutDownRequested          = false;
        registrationInfo           = RegistrationInfo();
        fileDescriptorToUnregister = -1;
        clientsReadyToRead.clear();
        clientsReadyToWrite.clear();
    }
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_EVENTS_IO_WORKINFO_H_
