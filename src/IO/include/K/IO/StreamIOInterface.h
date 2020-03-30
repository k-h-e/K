#ifndef K_IO_STREAMIOINTERFACE_H_
#define K_IO_STREAMIOINTERFACE_H_

#include <K/IO/StreamInputInterface.h>
#include <K/IO/StreamOutputInterface.h>

namespace K {
namespace IO {

//! Interface to entities providing stream I/O.
/*!
 *  \ingroup IO
 */
class StreamIOInterface : public virtual StreamInputInterface,
                          public virtual StreamOutputInterface {
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMIOINTERFACE_H_
