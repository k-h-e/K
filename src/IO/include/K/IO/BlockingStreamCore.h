#ifndef K_IO_BLOCKINGSTREAMCORE_H_
#define K_IO_BLOCKINGSTREAMCORE_H_

#include <K/IO/BlockingStreamInterface.h>

namespace K {
namespace IO {

//! Base class to blocking streams.
class BlockingStreamCore : public virtual BlockingStreamInterface {
  public:
    bool ReadItem(void *outItem, int itemSize) override;
    bool WriteItem(const void *item, int itemSize) override;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BLOCKINGSTREAMCORE_H_
