#ifndef K_IO_IOTOOLS_H_
#define K_IO_IOTOOLS_H_

#include <string>

namespace K {
namespace Core {
    class Interface;
    class ReadableStreamInterface;
}
}

namespace K {
//! I/O.
namespace IO {

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

BlockingStreamInterface &operator>>(BlockingStreamInterface &stream, uint8_t &value);
BlockingStreamInterface &operator>>(BlockingStreamInterface &stream, uint32_t &value);
BlockingStreamInterface &operator>>(BlockingStreamInterface &stream, float &value);
BlockingStreamInterface &operator>>(BlockingStreamInterface &stream, double &value);
BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, const std::string &text);
BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, uint8_t value);
BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, uint32_t value);
BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, float value);
BlockingStreamInterface &operator<<(BlockingStreamInterface &stream, double value);

//! Reads an integer.
/*!
 *  In case of failure, the output item will be undefined and error state or EOF will be raised on the stream.
 */
void Read(BlockingStreamInterface *stream, int *outValue);
//! Reads an 8-bit unsigned integer.
/*!
 *  In case of failure, the output item will be undefined and error state or EOF will be raised on the stream.
 */
void Read(BlockingStreamInterface *stream, uint8_t *outValue);
//! Reads a 32-bit unsigned integer.
/*!
 *  In case of failure, the output item will be undefined and error state or EOF will be raised on the stream.
 */
void Read(BlockingStreamInterface *stream, uint32_t *outValue);
//! Reads a single precision floating point number.
/*!
 *  In case of failure, the output item will be undefined and error state or EOF will be raised on the stream.
 */
void Read(BlockingStreamInterface *stream, float *outValue);
//! Reads a double precision floating point number.
/*!
 *  In case of failure, the output item will be undefined and error state or EOF will be raised on the stream.
 */
void Read(BlockingStreamInterface *stream, double *outValue);
//! Reads a string.
/*!
 *  Encountering the specified delimiter or EOF will terminate reading.
 *
 *  In case of failure, the output item will be undefined, and error state will be raised on the stream.
 */
void Read(BlockingStreamInterface *stream, char delimiter, std::string *outString);
//! Reads a string.
/*!
 *  Encountering a character not in the specified valid set or EOF will terminate reading. The next read on the stream
 *  will happen at the offending position.
 *
 *  In case of failure, the output string will be undefined and error state will be raised on the stream.
 *
 *  \param readOther
 *  If set, the complement of the specified set will be treated as the valid characters.
 */
void Read(SeekableBlockingStreamInterface *stream, const std::string &validCharacters, bool readOther,
           std::string *outString);
//! Skips the specified characters.
/*!
 *  Encountering a character not in the specified skip set or EOF will terminate reading. The next read on the stream
 *  will happen at the offending position.
 *
 *  In case of failure, error state will be raised on the stream.
 *
 *  \param skipOther
 *  If set, the complement of the specified set will be treated as the characters to skip.
 */
void Skip(SeekableBlockingStreamInterface *stream, const std::string &charactersToSkip, bool skipOther);
//! Writes the specified <c>uint8</c> value.
/*!
 *  In case of failure, error state will be raised on the stream.
 */
void Write(BlockingStreamInterface *stream, uint8_t value);
//! Writes the specified <c>uint32</c> value.
/*!
 *  In case of failure, error state will be raised on the stream.
 */
void Write(BlockingStreamInterface *stream, uint32_t value);
//! Writes the specified single-precision floating point number.
/*!
 *  In case of failure, error state will be raised on the stream.
 */
void Write(BlockingStreamInterface *stream, float value);
//! Writes the specified double-precision floating point number.
/*!
 *  In case of failure, error state will be raised on the stream.
 */
void Write(BlockingStreamInterface *stream, double value);
//! Writes the specified string.
/*!
 *  In case of failure, error state will be raised on the stream.
 */
void Write(BlockingStreamInterface *stream, const std::string &text);

}    // Namespace IO.
}    // namespace K.

#endif    // K_IO_IOTOOLS_H_
