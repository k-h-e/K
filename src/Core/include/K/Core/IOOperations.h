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
        class BinaryReaderInterface;
        class BinaryWriterInterface;
        class BlockingReadInterface;
        class BlockingWriteInterface;
        class TextWriterInterface;
    }
}

namespace K {
namespace Core {

BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, int &value);
BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, uint8_t &value);
BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, uint32_t &value);
BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, float &value);
BinaryReaderInterface &operator>>(BinaryReaderInterface &reader, double &value);

BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, int value);
BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, uint8_t value);
BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, uint32_t value);
BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, float value);
BinaryWriterInterface &operator<<(BinaryWriterInterface &writer, double value);

TextWriterInterface &operator<<(TextWriterInterface &writer, const std::string &text);

//! Reads a binary item of specified size (in bytes).
/*!
 *  \return <c>false</c> in case of failure.
 */
bool ReadItem(BlockingReadInterface *stream, void *item, int itemSize);
//! Writes a binary item of specified size (in bytes).
/*!
 *  \return <c>false</c> in case of failure.
 */
bool WriteItem(BlockingWriteInterface *stream, const void *item, int itemSize);

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_IOOPERATIONS_H_
