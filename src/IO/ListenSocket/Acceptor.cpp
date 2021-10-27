#include "Acceptor.h"

#include <netinet/in.h>
#include <K/Core/Log.h>
#include "SharedState.h"

using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

ListenSocket::Acceptor::Acceptor(int fd, SharedState *shared)
        : fd_(fd),
          shared_(shared) {
    // Nop.
}

void ListenSocket::Acceptor::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int fd = accept(fd_, (struct sockaddr *)&clientAddress, &clientAddressSize);
    if (fd != -1) {
        shared_->OnConnectionAccepted(fd);
        Log::Print(Log::Level::Debug, this, [&]{ return "socket " + to_string(fd) + " accepted connection"; });
    }

    Log::Print(Log::Level::Debug, this, []{ return "exiting"; });
}

}    // Namesapce IO.
}    // Namespace K.
