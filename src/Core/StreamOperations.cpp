#include <K/Core/StreamOperations.h>

#include <K/Core/SeekableBlockingIOStreamInterface.h>

using std::string;
using K::Core::BlockingIOStreamInterface;
using K::Core::SeekableBlockingIOStreamInterface;

namespace K {
namespace Core {

BlockingIOStreamInterface &operator>>(BlockingIOStreamInterface &stream, uint8_t &value) {
    Read(&stream, &value);
    return stream;
}

BlockingIOStreamInterface &operator>>(BlockingIOStreamInterface &stream, uint32_t &value) {
    Read(&stream, &value);
    return stream;
}

BlockingIOStreamInterface &operator>>(BlockingIOStreamInterface &stream, float &value) {
    Read(&stream, &value);
    return stream;
}

BlockingIOStreamInterface &operator>>(BlockingIOStreamInterface &stream, double &value) {
    Read(&stream, &value);
    return stream;
}

BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, const std::string &text) {
    Write(&stream, text);
    return stream;
}

BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, uint8_t value) {
    Write(&stream, value);
    return stream;
}

BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, uint32_t value) {
    Write(&stream, value);
    return stream;
}

BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, float value) {
    Write(&stream, value);
    return stream;
}

BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, double value) {
    Write(&stream, value);
    return stream;
}

void Read(BlockingIOStreamInterface *stream, int *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingIOStreamInterface *stream, uint8_t *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingIOStreamInterface *stream, uint32_t *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingIOStreamInterface *stream, float *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingIOStreamInterface *stream, double *outValue) {
    stream->ReadItem(outValue, sizeof(*outValue));
}

void Read(BlockingIOStreamInterface *stream, char delimiter, string *outString) {
    outString->clear();
    if (!stream->ReadFailed()) {
        while (true) {
            char character;
            stream->ReadItem(&character, sizeof(character));
            if (stream->ReadFailed()) {
                if (stream->Eof()) {
                    stream->ClearReadFailed();
                }
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
}

void Read(SeekableBlockingIOStreamInterface *stream, const string &validCharacters, bool readOther,
           string *outString) {
    outString->clear();
    if (!stream->ReadFailed()) {
        while (true) {
            int64_t position = stream->StreamPosition();

            char character;
            stream->ReadItem(&character, sizeof(character));
            if (stream->ReadFailed()) {
                if (stream->Eof()) {
                    stream->ClearReadFailed();
                }
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
}

void Skip(SeekableBlockingIOStreamInterface *stream, const string &charactersToSkip, bool skipOther) {
    if (!stream->ReadFailed()) {
        while (true) {
            int64_t position = stream->StreamPosition();

            char character;
            stream->ReadItem(&character, sizeof(character));
            if (stream->ReadFailed()) {
                if (stream->Eof()) {
                    stream->ClearReadFailed();
                }
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
}

void Write(BlockingIOStreamInterface *stream, uint8_t value) {
    stream->WriteItem(&value, sizeof(value));
}

void Write(BlockingIOStreamInterface *stream, uint32_t value) {
    stream->WriteItem(&value, sizeof(value));
}

void Write(BlockingIOStreamInterface *stream, float value) {
    stream->WriteItem(&value, sizeof(value));
}

void Write(BlockingIOStreamInterface *stream, double value) {
    stream->WriteItem(&value, sizeof(value));
}

void Write(BlockingIOStreamInterface *stream, const string &text) {
    int size = static_cast<int>(text.size() * sizeof(string::value_type));
    if (size > 0) {
        stream->WriteItem(&text[0], size);
    }
}

}    // Namespace Core.
}    // namespace K.
