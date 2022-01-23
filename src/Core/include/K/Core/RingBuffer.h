////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    RingBuffer(const RingBuffer &other)            = delete;
    RingBuffer &operator=(const RingBuffer &other) = delete;
    RingBuffer(RingBuffer &&other)                 = delete;
    RingBuffer &operator=(RingBuffer &&other)      = delete;

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
    //! Puts back (a copy of) the specified data into the buffer.
    /*!
     *  Intended for data that has been previously taken out of the buffer via <c>Get()</c>, but could not be consumed
     *  by the client. The put back data will be the first to be delivered with the next call to <c>Get()</c>.
     *
     *  The buffer grows as needed.
     */
    void PutBack(const void *data, int dataSize);

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
