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

bool CanRead(StreamInterface *stream) {
    return (!stream->ErrorState() && !stream->Eof());
}

bool Read(ItemReadInterface *stream, int *outValue) {
    return stream->ReadItem(outValue, sizeof(*outValue));
}

bool Read(BlockingStreamInterface *stream, char delimiter, string *outString) {
    outString->clear();
    while (true) {
        char character;
        if (!stream->ReadItem(&character, sizeof(character))) {
            return (stream->Eof() && !stream->ErrorState() && !outString->empty());
        }
        else if (character == delimiter) {
            return true;
        }
        else {
            outString->push_back(character);
        }
    }
}

bool ReadUntil(ItemReadInterface *stream, const std::string &delimiters, std::string *outString,
               char *outEncounteredDelimiter) {
    outString->clear();
    while (true) {
        char character;
        if (!stream->ReadItem(&character, sizeof(character))) {
            return false;
        }
        else if (delimiters.find(character) != string::npos) {
            if (outEncounteredDelimiter) {
                *outEncounteredDelimiter = character;
            }
            return true;
        }
        else {
            outString->push_back(character);
        }
    }
}

bool Skip(ItemReadInterface *stream, const string &charactersToSkip, char *outCharacter) {
    while (true) {
        char character;
        if (!stream->ReadItem(&character, sizeof(character))) {
            return false;
        }
        else if (charactersToSkip.find(character) == string::npos) {
            if (outCharacter) {
                *outCharacter = character;
            }
            return true;
        }
    }
}

bool SkipUntil(ItemReadInterface *stream, char character) {
    while (true) {
        char aCharacter;
        if (!stream->ReadItem(&aCharacter, sizeof(aCharacter))) {
            return false;
        }
        else if (aCharacter == character) {
            return true;
        }
    }
}

}    // Namespace IO.
}    // namespace K.
