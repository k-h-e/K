#include <K/Core/IOOperations.h>

#include <cassert>
#include <K/Core/BlockingInStreamInterface.h>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/StringTools.h>

using std::string;

namespace K {
namespace Core {

void ReadItem(BlockingInStreamInterface *stream, void *item, int itemSize) {
    assert(itemSize > 0);
    uint8_t *dest   = static_cast<uint8_t *>(item);
    int     numLeft = itemSize;
    while (numLeft) {
        int numRead = stream->ReadBlocking(dest, numLeft);
        if (numRead) {
            dest    += numRead;
            numLeft -= numRead;
        } else {
            assert (stream->ErrorState());
        }
    }
}

void WriteItem(BlockingOutStreamInterface *stream, const void *item, int itemSize) {
    assert(itemSize > 0);
    const uint8_t *source = static_cast<const uint8_t *>(item);
    int           numLeft = itemSize;
    while (numLeft) {
        int numWritten = stream->WriteBlocking(source, numLeft);
        if (numWritten) {
            source  += numWritten;
            numLeft -= numWritten;
        } else {
            assert (stream->ErrorState());
        }
    }
}

BlockingOutStreamInterface &operator<<(BlockingOutStreamInterface &stream, const std::string &value) {
    StringTools::Serialize(value, &stream);
    return stream;
}

}    // Namespace Core.
}    // namespace K.
