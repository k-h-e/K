/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/Deprecated/ListenSocket.h>

#include "SharedState.h"

using std::make_shared;
using std::shared_ptr;
using K::Core::ThreadPool;
using K::IO::Socket;

namespace K {
namespace IO {
namespace Deprecated {

ListenSocket::ListenSocket(int port, const shared_ptr<ConnectionIO> &connectionIO,
                           const shared_ptr<ThreadPool> &threadPool) {
    shared_ = make_shared<SharedState>(port, connectionIO, threadPool);
}

void ListenSocket::Register(HandlerInterface *handler, bool fileDescriptorMode) {
    shared_->Register(handler, fileDescriptorMode);
}

bool ListenSocket::ErrorState() const {
    return shared_->ErrorState();
}

}    // Namespace Deprecated.
}    // Namesapce IO.
}    // Namespace K.
