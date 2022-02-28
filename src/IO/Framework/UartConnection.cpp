#include <K/IO/Framework/UartConnection.h>

using std::shared_ptr;
using std::string;
using K::Core::Result;
using K::Core::Framework::RunLoop;
using K::IO::ConnectionIO;

namespace K {
namespace IO {
namespace Framework {

UartConnection::UartConnection(
    const string &deviceFileName, UartTools::BaudRate baudRate, const shared_ptr<RunLoop> &runLoop,
    const shared_ptr<ConnectionIO> &connectionIO)
        : Connection(UartTools::OpenUart(deviceFileName, baudRate), bufferSizeConstraint, nullptr, runLoop,
                     connectionIO) {
    // Nop.
}

UartConnection::UartConnection(
    const string &deviceFileName, UartTools::BaudRate baudRate, const shared_ptr<Result> &resultAcceptor,
    const shared_ptr<RunLoop> &runLoop, const shared_ptr<ConnectionIO> &connectionIO)
        : Connection(UartTools::OpenUart(deviceFileName, baudRate), bufferSizeConstraint, resultAcceptor, runLoop,
                     connectionIO) {
    // Nop.
}

}    // Namesapce Framework.
}    // Namesapce IO.
}    // Namespace K.
