/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/UartConnection.h>

using std::shared_ptr;
using std::string;
using K::Core::IoBuffers;
using K::Core::RunLoop;
using K::IO::ConnectionIO;

namespace K {
namespace IO {

UartConnection::UartConnection(
    const string &deviceFileName, Uart::BaudRate baudRate, const shared_ptr<RunLoop> &runLoop,
    const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<IoBuffers> &ioBuffers)
        : Connection{Uart::Open(deviceFileName, baudRate), bufferSizeConstraint, runLoop, connectionIO, ioBuffers} {
    // Nop.
}

}    // Namesapce IO.
}    // Namespace K.
