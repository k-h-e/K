/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_UARTCONNECTION_H_
#define K_IO_UARTCONNECTION_H_

#include <K/IO/Connection.h>
#include <K/IO/Uart.h>

namespace K {
    namespace IO {
        class ConnectionIO;
    }
}

namespace K {
namespace IO {

//! <c>RunLoop</c>-enabled UART (serial) connection.
class UartConnection : public Connection {
  public:
    //! Establishes a UART (serial) connection via the specified serial device.
    UartConnection(
        const std::string &deviceFileName, Uart::BaudRate baudRate, const std::shared_ptr<Core::RunLoop> &runLoop,
        const std::shared_ptr<K::IO::ConnectionIO> &connectionIO, const std::shared_ptr<Core::IoBuffers> &ioBuffers);
    UartConnection(const UartConnection &other)             = delete;
    UartConnection &operator=(const UartConnection &other)  = delete;
    UartConnection(const UartConnection &&other)            = delete;
    UartConnection &operator=(const UartConnection &&other) = delete;

  private:
    static const int bufferSizeConstraint = 4096;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_UARTCONNECTION_H_
