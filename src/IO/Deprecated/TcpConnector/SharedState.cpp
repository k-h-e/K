/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "SharedState.h"

using std::mutex;
using std::string;
using std::unique_lock;

namespace K {
namespace IO {
namespace Deprecated {

TcpConnector::SharedState::SharedState()
        : connectorRunning_{true} {
    // Nop.
}

void TcpConnector::SharedState::WaitForConnectorFinished() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    while (connectorRunning_) {
        stateChanged_.wait(critical);
    }
}    // ......................................................................................... critical section, end.

void TcpConnector::SharedState::OnCompletion(int completionId) {
    (void)completionId;
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    connectorRunning_ = false;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.
