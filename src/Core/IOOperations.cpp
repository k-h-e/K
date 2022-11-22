#include <K/Core/IOOperations.h>

#include <cassert>
#include <K/Core/BinaryReaderInterface.h>
#include <K/Core/BinaryWriterInterface.h>
#include <K/Core/BlockingReadInterface.h>
#include <K/Core/BlockingWriteInterface.h>
#include <K/Core/StringTools.h>
#include <K/Core/TextWriterInterface.h>

using std::string;

namespace K {
namespace Core {

BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, int &value) {
    value = reader.ReadInt();
    return reader;
}

BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, uint8_t &value) {
    value = reader.ReadUInt8();
    return reader;
}

BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, uint32_t &value) {
    value = reader.ReadUInt32();
    return reader;
}

BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, uint64_t &value) {
    reader.ReadItem(&value, sizeof(value));
    return reader;
}

BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, float &value) {
    value = reader.ReadFloat();
    return reader;
}

BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, double &value) {
    value = reader.ReadDouble();
    return reader;
}

BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, string &text) {
    StringTools::Deserialize(&text, &reader);
    return reader;
}

BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, int value) {
    writer.Write(value);
    return writer;
}

BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, uint8_t value) {
    writer.Write(value);
    return writer;
}

BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, uint32_t value) {
    writer.Write(value);
    return writer;
}

BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, uint64_t value) {
    writer.WriteItem(&value, sizeof(value));
    return writer;
}

BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, float value) {
    writer.Write(value);
    return writer;
}

BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, double value) {
    writer.Write(value);
    return writer;
}

BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, const string &text) {
    StringTools::Serialize(text, &writer);
    return writer;
}

TextWriterInterface &operator<<(TextWriterInterface &writer, const std::string &text) {
    writer.Write(text);
    return writer;
}

bool ReadItem(BlockingReadInterface *stream, void *item, int itemSize) {
    assert(itemSize > 0);
    uint8_t *dest   = static_cast<uint8_t *>(item);
    int     numLeft = itemSize;
    while (numLeft) {
        int numRead = stream->ReadBlocking(dest, numLeft);
        if (numRead) {
            dest    += numRead;
            numLeft -= numRead;
        } else {
            return false;
        }
    }

    return true;
}

bool WriteItem(BlockingWriteInterface *stream, const void *item, int itemSize) {
    assert(itemSize > 0);
    const uint8_t *source = static_cast<const uint8_t *>(item);
    int           numLeft = itemSize;
    while (numLeft) {
        int numWritten = stream->WriteBlocking(source, numLeft);
        if (numWritten) {
            source  += numWritten;
            numLeft -= numWritten;
        } else {
            return false;
        }
    }

    return true;
}

}    // Namespace Core.
}    // namespace K.
