////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_STREAMOPERATIONS_H_
#define K_CORE_STREAMOPERATIONS_H_

#include <string>

namespace K {
    namespace Core {
        class BlockingIOStreamInterface;
        class SeekableBlockingIOStreamInterface;
    }
}

namespace K {
namespace Core {

BlockingIOStreamInterface &operator>>(BlockingIOStreamInterface &stream, uint8_t &value);
BlockingIOStreamInterface &operator>>(BlockingIOStreamInterface &stream, uint32_t &value);
BlockingIOStreamInterface &operator>>(BlockingIOStreamInterface &stream, float &value);
BlockingIOStreamInterface &operator>>(BlockingIOStreamInterface &stream, double &value);
BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, const std::string &text);
BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, uint8_t value);
BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, uint32_t value);
BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, float value);
BlockingIOStreamInterface &operator<<(BlockingIOStreamInterface &stream, double value);

//! Reads an integer.
/*!
 *  Failure behavior as with <c>Core::ItemReadInterface::ReadItem()</c>.
 */
void Read(BlockingIOStreamInterface *stream, int *outValue);
//! Reads an 8-bit unsigned integer.
/*!
 *  Failure behavior as with <c>Core::ItemReadInterface::ReadItem()</c>.
 */
void Read(BlockingIOStreamInterface *stream, uint8_t *outValue);
//! Reads a 32-bit unsigned integer.
/*!
 *  Failure behavior as with <c>Core::ItemReadInterface::ReadItem()</c>.
 */
void Read(BlockingIOStreamInterface *stream, uint32_t *outValue);
//! Reads a single precision floating point number.
/*!
 *  Failure behavior as with <c>Core::ItemReadInterface::ReadItem()</c>.
 */
void Read(BlockingIOStreamInterface *stream, float *outValue);
//! Reads a double precision floating point number.
/*!
 *  Failure behavior as with <c>Core::ItemReadInterface::ReadItem()</c>.
 */
void Read(BlockingIOStreamInterface *stream, double *outValue);
//! Reads a string.
/*!
 *  Encountering the specified delimiter or EOF will terminate reading.
 *
 *  Failure behavior as with <c>Core::ItemReadInterface::ReadItem()</c>.
 */
void Read(BlockingIOStreamInterface *stream, char delimiter, std::string *outString);
//! Reads a string.
/*!
 *  Encountering a character not in the specified valid set or EOF will terminate reading. The next read on the stream
 *  will happen at the offending position.
 *
 *  Failure behavior as with <c>Core::ItemReadInterface::ReadItem()</c>.
 *
 *  \param readOther
 *  If set, the complement of the specified set will be treated as the valid characters.
 */
void Read(SeekableBlockingIOStreamInterface *stream, const std::string &validCharacters, bool readOther,
           std::string *outString);
//! Skips the specified characters.
/*!
 *  Encountering a character not in the specified skip set or EOF will terminate reading. The next read on the stream
 *  will happen at the offending position.
 *
 *  Failure behavior as with <c>Core::ItemReadInterface::ReadItem()</c>.
 *
 *  \param skipOther
 *  If set, the complement of the specified set will be treated as the characters to skip.
 */
void Skip(SeekableBlockingIOStreamInterface *stream, const std::string &charactersToSkip, bool skipOther);
//! Writes the specified <c>uint8</c> value.
/*!
 *  Failure behavior as with <c>Core::ItemWriteInterface::WriteItem()</c>.
 */
void Write(BlockingIOStreamInterface *stream, uint8_t value);
//! Writes the specified <c>uint32</c> value.
/*!
 *  Failure behavior as with <c>Core::ItemWriteInterface::WriteItem()</c>.
 */
void Write(BlockingIOStreamInterface *stream, uint32_t value);
//! Writes the specified single-precision floating point number.
/*!
 *  Failure behavior as with <c>Core::ItemWriteInterface::WriteItem()</c>.
 */
void Write(BlockingIOStreamInterface *stream, float value);
//! Writes the specified double-precision floating point number.
/*!
 *  Failure behavior as with <c>Core::ItemWriteInterface::WriteItem()</c>.
 */
void Write(BlockingIOStreamInterface *stream, double value);
//! Writes the specified string.
/*!
 *  Failure behavior as with <c>Core::ItemWriteInterface::WriteItem()</c>.
 */
void Write(BlockingIOStreamInterface *stream, const std::string &text);

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_STREAMOPERATIONS_H_
