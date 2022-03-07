#include <K/IO/BlockingIOStreamCore.h>

#include <cstdint>

namespace K {
namespace IO {

void BlockingIOStreamCore::ReadItem(void *item, int itemSize) {
    uint8_t *target      = static_cast<uint8_t *>(item);
    int     numRemaining = itemSize;
    while ((numRemaining > 0) && !ReadFailed()) {
        int num = ReadBlocking(target, numRemaining);
        if (num > 0) {
            target       += num;
            numRemaining -= num;
        }
    }
}

void BlockingIOStreamCore::WriteItem(const void *item, int itemSize) {
    const uint8_t *source      = static_cast<const uint8_t *>(item);
    int           numRemaining = itemSize;
    while ((numRemaining > 0) && !WriteFailed()) {
        int num = WriteBlocking(source, numRemaining);
        if (num > 0) {
            source       += num;
            numRemaining -= num;
        }
    }
}

}    // Namespace IO.
}    // Namespace K.
