/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_UART_H_
#define K_IO_UART_H_

#include <optional>
#include <string>

namespace K {
namespace IO {

// Various UART functions.
class Uart {
  public:
    enum class BaudRate { Baud4800,
                          Baud9600,
                          Baud38400,
                          Baud921600 };

    //! Opens the specified UART interface with the specified baud rate.
    /*!
     *  \return <c>nullopt</c> in case of failure.
     */
    static std::optional<int> Open(const std::string &deviceFileName, BaudRate baudRate);
};

}    // Namespace IO.
}    // namespace K.

#endif    // K_IO_UART_H_
