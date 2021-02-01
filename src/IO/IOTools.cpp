#include <K/IO/IOTools.h>

#include <unistd.h>
#include <cerrno>
#include <K/Core/Log.h>
#include <K/IO/BlockingStreamInterface.h>

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

bool Read(ItemReadInterface *reader, int *outValue) {
    return reader->ReadItem(outValue, sizeof(*outValue));
}

bool Read(ItemReadInterface *reader, char delimiter, string *outString) {
    outString->clear();
    char character;
    while (true) {
        if (!reader->ReadItem(&character, sizeof(character))) {
            return (reader->Eof() && !reader->ErrorState());
        }
        else if (character == delimiter) {
            return true;
        }
        else {
            outString->push_back(character);
        }
    }
}

bool SkipTo(ItemReadInterface *reader, char delimiter) {
    char character;
    while (true) {
        if (!reader->ReadItem(&character, sizeof(character))) {
            return false;
        }
        else if (character == delimiter) {
            return true;
        }
    }
}

}    // Namespace IO.
}    // namespace K.
