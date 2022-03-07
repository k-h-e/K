#ifndef K_IO_BLOCKINGIOSTREAMCORE_H_
#define K_IO_BLOCKINGIOSTREAMCORE_H_

#include <K/Core/BlockingIOStreamInterface.h>

namespace K {
namespace IO {

//! Base class to blocking input/output streams.
class BlockingIOStreamCore : public virtual Core::BlockingIOStreamInterface {
  public:
    void ReadItem(void *item, int itemSize) override;
    void WriteItem(const void *item, int itemSize) override;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BLOCKINGIOSTREAMCORE_H_
