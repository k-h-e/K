////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_IOOPERATIONS_H_
#define K_CORE_IOOPERATIONS_H_

#include <string>

namespace K {
    namespace Core {
        class BlockingInStreamInterface;
        class BlockingOutStreamInterface;
    }
}

namespace K {
namespace Core {

//! Reads a binary item of specified size (in bytes).
void ReadItem(BlockingInStreamInterface *stream, void *item, int itemSize);

template<typename T>
BlockingInStreamInterface &operator>>(BlockingInStreamInterface &stream, T &outValue) {
    ReadItem(&stream, &outValue, sizeof(T));
    return stream;
}

BlockingInStreamInterface &operator>>(BlockingInStreamInterface &stream, std::string &outValue);

//! Writes a binary item of specified size (in bytes).
void WriteItem(BlockingOutStreamInterface *stream, const void *item, int itemSize);

template<typename T>
BlockingOutStreamInterface &operator<<(BlockingOutStreamInterface &stream, const T &value) {
    WriteItem(&stream, &value, sizeof(T));
    return stream;
}

BlockingOutStreamInterface &operator<<(BlockingOutStreamInterface &stream, const std::string &value);

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_IOOPERATIONS_H_
