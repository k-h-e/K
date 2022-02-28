#ifndef K_IO_UARTTOOLS_H_
#define K_IO_UARTTOOLS_H_

#include <optional>
#include <string>

namespace K {
namespace IO {

class UartTools {
  public:
    enum class BaudRate { Baud4800,
                          Baud9600,
                          Baud38400 };

    //! Opens the specified UART interface with the specified baud rate.
    /*!
     *  \return <c>nullopt</c> in case of failure.
     */
    static std::optional<int> OpenUart(const std::string &deviceFileName, BaudRate baudRate);
};

}    // Namespace IO.
}    // namespace K.

#endif    // K_IO_UARTTOOLS_H_
