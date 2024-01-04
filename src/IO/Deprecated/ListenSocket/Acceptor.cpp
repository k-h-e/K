/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "Acceptor.h"

#include <netinet/in.h>
#include <K/Core/Log.h>
#include <K/IO/IOTools.h>
#include "SharedState.h"

using std::to_string;
using K::Core::Log;
using K::IO::IOTools;

namespace K {
namespace IO {
namespace Deprecated {

ListenSocket::Acceptor::Acceptor(int port, SharedState *shared)
        : shared_(shared),
          error_(true) {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ != -1) {
        struct sockaddr_in address = {};
        address.sin_family      = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port        = htons(static_cast<uint16_t>(port));
        if (!bind(fd_, (struct sockaddr *)&address, sizeof(address))) {
            if (!listen(fd_, 4)) {
                error_ = false;
                Log::Print(Log::Level::Debug, this, [&]{ return "socket " + to_string(fd_) + " listening on port "
                    + to_string(port); });
            }
        }
    }

    if (error_) {
        Close();
        Log::Print(Log::Level::Error, this, [=]{ return "failed to listen on port " + to_string(port); });
    }
}

ListenSocket::Acceptor::~Acceptor() {
    Close();
}

void ListenSocket::Acceptor::SetErrorState() {
    error_ = true;
    Close();
}

void ListenSocket::Acceptor::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, []{ return "spawning..."; });

    if (!error_) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);

        bool done = false;
        while (!done) {
            int fd = accept(fd_, (struct sockaddr *)&clientAddress, &clientAddressSize);
            if (fd != -1) {
                Log::Print(Log::Level::Debug, this, [&]{
                    return "socket " + to_string(fd_) + " accepted connection " + to_string(fd);
                });
                shared_->OnConnectionAccepted(fd);
                done = true;
            } else {
                if (errno != EINTR) {
                    Log::Print(Log::Level::Warning, this, [&]{
                        return "socket " + to_string(fd_) + " reported failure on accept";
                    });
                    SetErrorState();
                    done = true;
                }
            }
        }
    }

    if (error_) {
        shared_->ReportError();
    }

    Log::Print(Log::Level::Debug, this, []{ return "exiting"; });
}

void ListenSocket::Acceptor::Close() {
    if (fd_ != -1) {
        IOTools::CloseFileDescriptor(fd_, this);
        fd_ = -1;
    }
}

}    // Namespace Deprecated.
}    // Namesapce IO.
}    // Namespace K.
