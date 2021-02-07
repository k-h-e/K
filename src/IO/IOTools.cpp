#include <K/IO/IOTools.h>

#include <unistd.h>
#include <cerrno>
#include <K/Core/Log.h>
#include <K/IO/SeekableBlockingStreamInterface.h>

using std::string;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

const char *IOTools::whiteSpace = " \t\r\n";

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

bool Read(ItemReadInterface *stream, int *outValue) {
    return stream->ReadItem(outValue, sizeof(*outValue));
}

bool Read(BlockingStreamInterface *stream, char delimiter, string *outString) {
    outString->clear();
    char character;
    while (true) {
        if (!stream->ReadItem(&character, sizeof(character))) {
            return (stream->Eof() && !stream->ErrorState());
        }
        else if (character == delimiter) {
            return true;
        }
        else {
            outString->push_back(character);
        }
    }
}

bool Skip(SeekableBlockingStreamInterface *stream, const string &charactersToSkip, char *outNextCharacter) {
    char character;
    while (true) {
        int64_t position = stream->StreamPosition();
        if (!stream->ReadItem(&character, sizeof(character))) {
            return false;
        }
        else if (charactersToSkip.find(character) == string::npos) {
            *outNextCharacter = character;
            return stream->Seek(position);
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
