#ifndef K_IO_UARTCONNECTION_H_
#define K_IO_UARTCONNECTION_H_

#include <memory>
#include <K/IO/BufferedConnection.h>

namespace K {
namespace IO {

class ConnectionIO;

//! UART (serial) connection.
class UartConnection : public BufferedConnection {
  public:
    //! Establishes a UART (serial) connection via the specified serial device.
    UartConnection(const std::string &deviceFileName, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    UartConnection(const UartConnection &other)             = delete;
    UartConnection &operator=(const UartConnection &other)  = delete;
    UartConnection(const UartConnection &&other)            = delete;
    UartConnection &operator=(const UartConnection &&other) = delete;

  private:
    static const int bufferSizeThreshold = 4096;

    static int OpenUart(const std::string &deviceFileName);
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_UARTCONNECTION_H_
