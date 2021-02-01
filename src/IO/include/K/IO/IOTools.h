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

class IOTools {
  public:
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
bool Read(ItemReadInterface *reader, int *outValue);
//! Reads a string.
/*!
 *  Blocks until either the read is complete or it fails. The read is complete when the specified delimiter or EOF is
 *  reached.
 *
 * \return
 *  <c>false</c> in case of failure. The output item will then be undefined, and error state or EOF will be raised
 *  on the stream.
 */
bool Read(ItemReadInterface *reader, char delimiter, std::string *outString);
//! Reads and skips (ignores) bytes until the specified delimiter is encountered.
/*!
 * \return
 *  <c>false</c> in case of failure. Error state or EOF will then be raised on the stream.
 */
bool SkipTo(ItemReadInterface *reader, char delimiter);

}    // Namespace IO.
}    // namespace K.

#endif    // K_IO_IOTOOLS_H_
