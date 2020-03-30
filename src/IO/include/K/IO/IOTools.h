#ifndef K_IO_IOTOOLS_H_
#define K_IO_IOTOOLS_H_

namespace K {
namespace IO {

class StreamInputInterface;
class StreamOutputInterface;

//! I/O tools.
/*!
 *  \ingroup IO
 */
class IOTools {
  public:

};

//! Reads a binary item of specified size (in bytes).
/*!
 *  Don't use on nonblocking streams (would result in polling).
 *
 * \return
 *  <c>false</c> in case of failure. The output item will then be undefined, and error state will be raised on the
 *  stream.
 */
bool ReadItem(StreamInputInterface *stream, void *outItem, int itemSize);
//! Writes a binary item of specified size (in bytes).
/*!
 *  Don't use on nonblocking streams (would result in polling).
 *
 * \return
 *  <c>false</c> in case of failure. Error state will then be raised on the stream.
 */
bool WriteItem(StreamOutputInterface *stream, const void *item, int itemSize);

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_IOTOOLS_H_
