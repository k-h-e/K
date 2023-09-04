#include <K/IO/Uart.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

using std::nullopt;
using std::optional;
using std::string;

namespace K {
namespace IO {

optional<int> Uart::Open(const string &deviceFileName, BaudRate baudRate) {
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
            case BaudRate::Baud38400:
                baudRateFlag = B38400;
                break;
            case BaudRate::Baud921600:
#if defined(K_PLATFORM_MAC) || defined(K_PLATFORM_IOS)
                baudRateFlag = 921600;
#else
                baudRateFlag = B921600;
#endif
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

    return nullopt;
}

}    // Namespace IO.
}    // namespace K.
