#ifndef K_IO_IOTOOLS_H_
#define K_IO_IOTOOLS_H_

#include <string>

namespace K {
namespace Core {
    class Interface;
}
}

namespace K {
namespace IO {

class StreamInterface;
class ItemReadInterface;
class ItemWriteInterface;
class BlockingStreamInterface;
class SeekableBlockingStreamInterface;

class IOTools {
  public:
    static const char *whiteSpace;

    //! Closes the specified UNIX file descriptor.
    /*!
     *  \return
     *  <c>false</c> in case of failure.
     */
    static bool CloseFileDescriptor(int fd, K::Core::Interface *loggingObject);
};

ItemWriteInterface &operator<<(ItemWriteInterface &stream, const std::string &text);
ItemWriteInterface &operator<<(ItemWriteInterface &stream, uint8_t value);
ItemWriteInterface &operator<<(ItemWriteInterface &stream, uint32_t value);
ItemWriteInterface &operator<<(ItemWriteInterface &stream, float value);
ItemWriteInterface &operator<<(ItemWriteInterface &stream, double value);

//! Returns <c>true</c> iff neither error nor EOF state are set on the specified stream.
bool Good(StreamInterface *stream);

//! Reads an integer.
/*!
 *  \return
 *  <c>false</c> in case of failure. The output item will then be undefined, and error state or EOF will be raised
 *  on the stream.
 */
bool Read(ItemReadInterface *stream, int *outValue);
//! Reads a string.
/*!
 *  \return
 *  <c>false</c> in case of failure. The output item will then be undefined, and error state or EOF will be raised
 *  on the stream (or both).
 */
bool Read(BlockingStreamInterface *stream, char delimiter, std::string *outString);
//! Reads a string.
/*!
 *  The read is (only) successful when one of the specified delimiters has been reached.
 *
 *  \param outEncounteredDelimiter
 *  If given will accept the delimiter that was encountered.
 *
 *  \return
 *  <c>false</c> in case of failure. The output items will then be undefined, and error state or EOF will be raised
 *  on the stream (or both).
 */
bool ReadUntil(ItemReadInterface *stream, const std::string &delimiters, std::string *outString,
               char *outEncounteredDelimiter);
//! Skips (ignores) bytes until a character other than those specified is encountered.
/*!
 *  \param outCharacter
 *  If given will accept the (not-skippable) character that was encountered.
 *
 * \return
 *  <c>false</c> in case of failure. Error state or EOF will then be raised on the stream (or both). The output
 *  parameter will then be undefined.
 */
bool Skip(ItemReadInterface *stream, const std::string &charactersToSkip, char *outCharacter);
//! Skips (ignores) bytes until the specified character is encountered.
/*!
 * \return
 *  <c>false</c> in case of failure. Error state or EOF will then be raised on the stream (or both).
 */
bool SkipUntil(ItemReadInterface *stream, char character);
//! Writes the specified string.
/*!
 *  \return
 *  <c>false</c> in case of failure. Error state will then be raised on the stream.
 */
bool Write(ItemWriteInterface *stream, const std::string &text);
//! Writes the specified <c>uint8</c> value.
/*!
 *  \return
 *  <c>false</c> in case of failure. Error state will then be raised on the stream.
 */
bool Write(ItemWriteInterface *stream, uint8_t value);
//! Writes the specified <c>uint32</c> value.
/*!
 *  \return
 *  <c>false</c> in case of failure. Error state will then be raised on the stream.
 */
bool Write(ItemWriteInterface *stream, uint32_t value);
//! Writes the specified single-precision floating point number.
/*!
 *  \return
 *  <c>false</c> in case of failure. Error state will then be raised on the stream.
 */
bool Write(ItemWriteInterface *stream, float value);
//! Writes the specified double-precision floating point number.
/*!
 *  \return
 *  <c>false</c> in case of failure. Error state will then be raised on the stream.
 */
bool Write(ItemWriteInterface *stream, double value);

}    // Namespace IO.
}    // namespace K.

#endif    // K_IO_IOTOOLS_H_
