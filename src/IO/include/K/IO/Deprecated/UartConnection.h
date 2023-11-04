/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_DEPRECATED_UARTCONNECTION_H_
#define K_IO_DEPRECATED_UARTCONNECTION_H_

#include <memory>
#include <K/IO/Uart.h>
#include <K/IO/Deprecated/BufferedConnection.h>

namespace K {
    namespace IO {
        class ConnectionIO;
    }
}

namespace K {
namespace IO {
namespace Deprecated {

//! UART (serial) connection.
class UartConnection : public BufferedConnection {
  public:
    //! Establishes a UART (serial) connection via the specified serial device.
    UartConnection(const std::string &deviceFileName, Uart::BaudRate baudRate,
                   const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    UartConnection(const UartConnection &other)             = delete;
    UartConnection &operator=(const UartConnection &other)  = delete;
    UartConnection(const UartConnection &&other)            = delete;
    UartConnection &operator=(const UartConnection &&other) = delete;

  private:
    static const int bufferSizeThreshold = 4096;
};

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DEPRECATED_UARTCONNECTION_H_
