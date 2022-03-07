#include <K/IO/IOTools.h>

#include <unistd.h>
#include <cerrno>
#include <K/Core/Log.h>

using std::string;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

const char *IOTools::whiteSpace = " \t\r\n";

bool IOTools::CloseFileDescriptor(int fd, Core::Interface *loggingObject) {
    while (true) {
        if (!close(fd)) {
            Log::Print(Log::Level::Debug, loggingObject, [&]{ return "fd " + to_string(fd) + " closed successfully"; });
            return true;
        }
        else {
            Log::Print(Log::Level::Warning, loggingObject, [&]{
                return "fd " + to_string(fd) + " closed with error, errno=" + to_string(errno);
            });
            if (errno != EINTR) {
                return false;
            }
        }
    }
}

}    // Namespace IO.
}    // namespace K.
