#include <K/IO/IOTools.h>

#include <unistd.h>
#include <cerrno>
#include <K/Core/Log.h>

using std::string;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

bool IOTools::CloseFileDescriptor(int fd, Core::Interface *loggingObject) {
    while (true) {
        Log::Print(Log::Level::Debug, loggingObject, [&]{ return "closing fd " + to_string(fd); });
        if (!close(fd)) {
            Log::Print(Log::Level::Debug, loggingObject, [&]{ return "fd " + to_string(fd) + " closed"; });
            return true;
        }
        else {
            Log::Print(Log::Level::Warning, loggingObject, [&]{ return "closing of fd " + to_string(fd)
                + "reported error, errno=" + to_string(errno); });
            if (errno != EINTR) {
                return false;
            }
        }
    }
}

}    // Namespace IO.
}    // namespace K.
