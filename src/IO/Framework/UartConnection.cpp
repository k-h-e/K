#include <K/IO/Framework/UartConnection.h>

using std::shared_ptr;
using std::string;
using K::Core::Framework::RunLoop;
using K::IO::ConnectionIO;

namespace K {
namespace IO {
namespace Framework {

UartConnection::UartConnection(
    const string &deviceFileName, Uart::BaudRate baudRate, const shared_ptr<RunLoop> &runLoop,
    const shared_ptr<ConnectionIO> &connectionIO)
        : Connection(Uart::Open(deviceFileName, baudRate), bufferSizeConstraint, runLoop, connectionIO) {
    // Nop.
}

}    // Namesapce Framework.
}    // Namesapce IO.
}    // Namespace K.
