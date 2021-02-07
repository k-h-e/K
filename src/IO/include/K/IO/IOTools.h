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

//! Reads an integer.
/*!
 *  Blocks until either the read is complete or it fails.
 *
 * \return
 *  <c>false</c> in case of failure. The output item will then be undefined, and error state or EOF will be raised
 *  on the stream.
 */
bool Read(ItemReadInterface *stream, int *outValue);
//! Reads a string.
/*!
 *  Blocks until either the read is complete or it fails. The read is complete when the specified delimiter or EOF is
 *  reached.
 *
 * \return
 *  <c>false</c> in case of failure. The output item will then be undefined, and error state or EOF will be raised
 *  on the stream.
 */
bool Read(BlockingStreamInterface *stream, char delimiter, std::string *outString);
//! Reads a string.
/*!
 *  Blocks until either the read is complete or it fails. The read is complete when one of the specified delimiters has
 *  been reached.
 *
 * \return
 *  <c>false</c> in case of failure. The output items will then be undefined, and error state or EOF will be raised
 *  on the stream.
 */
bool ReadUntil(BlockingStreamInterface *stream, const std::string &delimiters, std::string *outString,
               char *outEncounteredDelimiter);
//! Reads and skips (ignores) bytes until a character other than those specified is encountered.
/*!
 * \return
 *  <c>false</c> in case of failure. Error state or EOF will then be raised on the stream. The output parameter will
 *  then be undefined.
 */
bool Skip(SeekableBlockingStreamInterface *stream, const std::string &charactersToSkip, char *outNextCharacter);
//! Reads and skips (ignores) bytes until the specified delimiter is encountered.
/*!
 * \return
 *  <c>false</c> in case of failure. Error state or EOF will then be raised on the stream.
 */
bool SkipTo(ItemReadInterface *stream, char delimiter);

}    // Namespace IO.
}    // namespace K.

#endif    // K_IO_IOTOOLS_H_
