#include <K/IO/UartConnection.h>

#include <K/Core/Log.h>

using std::shared_ptr;
using std::string;
using K::Core::Result;
using K::IO::ConnectionIO;

namespace K {
namespace IO {

UartConnection::UartConnection(const string &deviceFileName, UartTools::BaudRate baudRate,
                               const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(UartTools::OpenUart(deviceFileName, baudRate), bufferSizeThreshold, nullptr,
                             connectionIO) {
    // Nop.
}

UartConnection::UartConnection(
    const string &deviceFileName, UartTools::BaudRate baudRate, const shared_ptr<Result> &resultAcceptor,
    const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(UartTools::OpenUart(deviceFileName, baudRate), bufferSizeThreshold, resultAcceptor,
                             connectionIO) {
    // Nop.
}

}    // Namesapce IO.
}    // Namespace K.
