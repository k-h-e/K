#include <K/IO/BlockingStreamCore.h>

#include <cstdint>

namespace K {
namespace IO {

bool BlockingStreamCore::ReadItem(void *outItem, int itemSize) {
    uint8_t *target      = static_cast<uint8_t *>(outItem);
    int     numRemaining = itemSize;
    while (numRemaining > 0) {
        int num = Read(target, numRemaining);
        if (num > 0) {
            target       += num;
            numRemaining -= num;
        }
        else {
            if (Error() || EndOfStream()) {
                return false;
            }
        }
    }
    return true;
}

bool BlockingStreamCore::WriteItem(const void *item, int itemSize) {
    const uint8_t *source      = static_cast<const uint8_t *>(item);
    int           numRemaining = itemSize;
    while (numRemaining > 0) {
        int num = Write(source, numRemaining);
        if (num > 0) {
            source       += num;
            numRemaining -= num;
        }
        else {
            if (Error()) {
                return false;
            }
        }
    }
    return true;
}

}    // Namespace IO.
}    // Namespace K.
