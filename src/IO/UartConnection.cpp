#include <K/IO/UartConnection.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <K/Core/Log.h>

using std::shared_ptr;
using std::string;
using K::IO::ConnectionIO;

namespace K {
namespace IO {

UartConnection::UartConnection(const string &deviceFileName, BaudRate baudRate,
                               const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(OpenUart(deviceFileName, baudRate), bufferSizeThreshold, connectionIO) {
    // Nop.
}

int UartConnection::OpenUart(const string &deviceFileName, BaudRate baudRate) {
    int fd = open(deviceFileName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd != -1) {
        tcflag_t baudRateFlag;
        switch (baudRate) {
            case BaudRate::Baud4800:
                baudRateFlag = B4800;
                break;
            case BaudRate::Baud9600:
                baudRateFlag = B9600;
                break;
            default:
                baudRateFlag = B9600;
                break;
        }

        struct termios options;
        if (!tcgetattr(fd, &options)) {
            options.c_cflag = baudRateFlag | CS8 | CLOCAL | CREAD;
            options.c_iflag = 0;
            options.c_oflag = 0;
            options.c_lflag = 0;
            if (!tcsetattr(fd, TCSANOW, &options)) {
                return fd;
            }
        }
    }

    return -1;
}

}    // Namesapce IO.
}    // Namespace K.
