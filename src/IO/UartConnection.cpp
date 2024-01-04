/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/UartConnection.h>

using std::shared_ptr;
using std::string;
using K::Core::RunLoop;
using K::IO::ConnectionIO;

namespace K {
namespace IO {

UartConnection::UartConnection(
    const string &deviceFileName, Uart::BaudRate baudRate, const shared_ptr<RunLoop> &runLoop,
    const shared_ptr<ConnectionIO> &connectionIO)
        : Connection(Uart::Open(deviceFileName, baudRate), bufferSizeConstraint, runLoop, connectionIO) {
    // Nop.
}

}    // Namesapce IO.
}    // Namespace K.
