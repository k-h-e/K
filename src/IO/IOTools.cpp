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
        if (!close(fd)) {
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

BlockingStreamInterface &operator>>(BlockingStreamInterface &stream, uint8_t &value) {
    Read(&stream, &value);
    return stream;
}

BlockingStreamInterface &operator>>(BlockingStreamInterface &stream, uint32_t &value) {
    Read(&stream, &value);
    return stream;
}

BlockingStreamInterface &operator>>(BlockingStreamInterface &stream, float &value) {
    Read(&stream, &value);
    return stream;
}

BlockingStreamInterface &operator>>(BlockingStreamInterface &stream, double &value) {
    Read(&stream, &value);
    return stream;
}

BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, const std::string &text) {
    Write(&stream, text);
    return stream;
}

BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, uint8_t value) {
    Write(&stream, value);
    return stream;
}

BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, uint32_t value) {
    Write(&stream, value);
    return stream;
}

BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, float value) {
    Write(&stream, value);
    return stream;
}

BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, double value) {
    Write(&stream, value);
    return stream;
}

void Read(BlockingStreamInterface *stream, int *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingStreamInterface *stream, uint8_t *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingStreamInterface *stream, uint32_t *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingStreamInterface *stream, float *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingStreamInterface *stream, double *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingStreamInterface *stream, char delimiter, string *outString) {
    outString->clear();
    while (true) {
        char character;
        stream->ReadItem(&character, sizeof(character));
        if (!stream->Good()) {
            return;
        }
        else if (character == delimiter) {
            return;
        }
        else {
            outString->push_back(character);
        }
    }
}

void Read(SeekableBlockingStreamInterface *stream, const string &validCharacters, bool readOther, string *outString) {
    outString->clear();
    while (true) {
        int64_t position = stream->StreamPosition();

        char character;
        stream->ReadItem(&character, sizeof(character));
        if (!stream->Good()) {
            return;
        }

        bool characterValid = (validCharacters.find(character) != string::npos);
        if (readOther) {
            characterValid = !characterValid;
        }

        if (!characterValid) {
            stream->Seek(position);
            return;
        }
        else {
            outString->push_back(character);
        }
    }
}

void Skip(SeekableBlockingStreamInterface *stream, const string &charactersToSkip, bool skipOther) {
    while (true) {
        int64_t position = stream->StreamPosition();

        char character;
        stream->ReadItem(&character, sizeof(character));
        if (!stream->Good()) {
            return;
        }

        bool skipCharacter = (charactersToSkip.find(character) != string::npos);
        if (skipOther) {
            skipCharacter = !skipCharacter;
        }

        if (!skipCharacter) {
            stream->Seek(position);
            return;
        }
    }
}

void Write(BlockingStreamInterface *stream, uint8_t value) {
    stream->WriteItem(&value, sizeof(value));
}

void Write(BlockingStreamInterface *stream, uint32_t value) {
    stream->WriteItem(&value, sizeof(value));
}

void Write(BlockingStreamInterface *stream, float value) {
    stream->WriteItem(&value, sizeof(value));
}

void Write(BlockingStreamInterface *stream, double value) {
    stream->WriteItem(&value, sizeof(value));
}

void Write(BlockingStreamInterface *stream, const string &text) {
    int size = static_cast<int>(text.size() * sizeof(string::value_type));
    if (size > 0) {
        stream->WriteItem(&text[0], size);
    }
}

}    // Namespace IO.
}    // namespace K.
