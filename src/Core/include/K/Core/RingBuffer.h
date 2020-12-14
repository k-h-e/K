#ifndef K_CORE_RINGBUFFER_H_
#define K_CORE_RINGBUFFER_H_

#include <cstdint>
#include <vector>

namespace K {
namespace Core {

//! Ring buffer with FIFO semantics.
class RingBuffer {
  public:
    RingBuffer();
    // Default copy/move, ok.

    //! Returns the number of bytes currently in the ring buffer.
    int Fill();
    //! Gets (and removes) at most <c>bufferSize</c> bytes from the buffer.
    /*!
     *  \return
     *  Number of bytes actually fetched from the buffer. <c>0</c> in case the buffer was empty.
     */
    int Get(void *outBuffer, int bufferSize);
    //! Puts (a copy of) the specified data in the buffer.
    /*!
     *  The buffer grows as needed.
     */
    void Put(const void *data, int dataSize);

  private:
    void Grow();

    std::vector<uint8_t> buffer_;
    int                  size_;
    int                  cursor_;
    int                  fill_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_RINGBUFFER_H_
