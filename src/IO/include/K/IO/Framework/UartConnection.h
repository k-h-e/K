#ifndef K_IO_FRAMEWORK_UARTCONNECTION_H_
#define K_IO_FRAMEWORK_UARTCONNECTION_H_

#include <K/IO/UartTools.h>
#include <K/IO/Framework/Connection.h>

namespace K {
    namespace IO {
        class ConnectionIO;
    }
}

namespace K {
namespace IO {
namespace Framework {

//! UART (serial) connection for use with a <c>Core::Framework::RunLoop</c>.
class UartConnection : public Connection {
  public:
    //! Establishes a UART (serial) connection via the specified serial device.
    UartConnection(
        const std::string &deviceFileName, UartTools::BaudRate baudRate,
        const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
        const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    //! Establishes a UART (serial) connection via the specified serial device.
    UartConnection(
        const std::string &deviceFileName, UartTools::BaudRate baudRate,
        const std::shared_ptr<Core::Result> &resultAcceptor, const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
        const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    UartConnection(const UartConnection &other)             = delete;
    UartConnection &operator=(const UartConnection &other)  = delete;
    UartConnection(const UartConnection &&other)            = delete;
    UartConnection &operator=(const UartConnection &&other) = delete;

  private:
    static const int bufferSizeConstraint = 4096;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_UARTCONNECTION_H_
