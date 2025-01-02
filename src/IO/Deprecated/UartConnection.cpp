/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/Deprecated/UartConnection.h>

#include <K/Core/Log.h>

using std::shared_ptr;
using std::string;
using K::Core::IoBuffers;
using K::IO::ConnectionIO;

namespace K {
namespace IO {
namespace Deprecated {

UartConnection::UartConnection(const string &deviceFileName, Uart::BaudRate baudRate,
                               const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<IoBuffers> &ioBuffers)
        : BufferedConnection{Uart::Open(deviceFileName, baudRate), bufferSizeThreshold, connectionIO, ioBuffers} {
    // Nop.
}

}    // Namespace Deprecated.
}    // Namesapce IO.
}    // Namespace K.
