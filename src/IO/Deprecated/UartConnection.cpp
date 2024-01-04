/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/Deprecated/UartConnection.h>

#include <K/Core/Log.h>

using std::shared_ptr;
using std::string;
using K::IO::ConnectionIO;

namespace K {
namespace IO {
namespace Deprecated {

UartConnection::UartConnection(const string &deviceFileName, Uart::BaudRate baudRate,
                               const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(Uart::Open(deviceFileName, baudRate), bufferSizeThreshold, connectionIO) {
    // Nop.
}

}    // Namespace Deprecated.
}    // Namesapce IO.
}    // Namespace K.
