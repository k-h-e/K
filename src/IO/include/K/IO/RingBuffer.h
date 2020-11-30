#ifndef K_IO_RINGBUFFER_H_
#define K_IO_RINGBUFFER_H_

#include <cstdint>
#include <vector>
#include <K/IO/SimpleNonBlockingStreamInterface.h>

namespace K {
namespace IO {

//! Ring buffer.
/*!
 *  It is guaranteed that Write() always accepts all data.
 */
class RingBuffer : public virtual SimpleNonBlockingStreamInterface {
  public:
    RingBuffer();
    // Default copy/move, ok.

    //! Returns the number of bytes currently in the ring buffer.
    int Fill();
    bool Error() override;
    int Read(void *outBuffer, int bufferSize) override;
    bool Eof() override;
    int Write(const void *data, int dataSize) override;

  private:
    void Grow();

    std::vector<uint8_t> buffer_;
    int                  size_;
    int                  cursor_;
    int                  fill_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_RINGBUFFER_H_
