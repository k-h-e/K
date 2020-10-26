#ifndef K_EVENTS_IO_WORKINFO_H_
#define K_EVENTS_IO_WORKINFO_H_

#include <K/IO/IO.h>

namespace K {
namespace IO {

struct RegistrationInfo {
    int                       fileDescriptor;
    IO::ReadHandlerInterface  *reader;
    IO::WriteHandlerInterface *writer;

    RegistrationInfo()
        : fileDescriptor(-1),
          reader(nullptr),
          writer(nullptr) {
        // Nop.
    }

    RegistrationInfo(int aFileDescriptor, IO::ReadHandlerInterface  *aReader, IO::WriteHandlerInterface *aWriter)
        : fileDescriptor(aFileDescriptor),
          reader(aReader),
          writer(aWriter) {
        // Nop.
    }

    // Default copy/move, okay.
};

//! Describes work for the I/O worker.
struct WorkInfo {
    bool             shutDownRequested;
    RegistrationInfo registrationInfo;
    int              fileDescriptorToUnregister;

    WorkInfo()
        : shutDownRequested(false),
          fileDescriptorToUnregister(-1) {
        // Nop.
    }
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_EVENTS_IO_WORKINFO_H_
