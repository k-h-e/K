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

class NonBlockingWriteInterface;

//! Ring buffer with FIFO semantics.
class RingBuffer {
  public:
    RingBuffer();
    RingBuffer(const RingBuffer &other)            = delete;
    RingBuffer &operator=(const RingBuffer &other) = delete;
    RingBuffer(RingBuffer &&other)                 = delete;
    RingBuffer &operator=(RingBuffer &&other)      = delete;

    //! Returns the number of bytes currently in the ring buffer.
    int Size() const;
    //! Tells whether the ring buffer is currently empty.
    bool Empty() const;
    //! Gets (and removes) at most <c>bufferSize</c> bytes from the buffer.
    /*!
     *  \return
     *  Number of bytes actually fetched from the buffer. <c>0</c> means the buffer was empty.
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
    //! Transfers data to the specified other ring buffer, as long as the other ring buffer's size does not exceed the
    //! size specified.
    void TransferTo(RingBuffer *other, int maxSize);
    //! Transfers data to the specified writer, as long as the writer accepts more data.
    void TransferTo(NonBlockingWriteInterface *writer);

  private:
    void Grow();

    std::vector<uint8_t> buffer_;
    int                  capacity_;
    int                  cursor_;
    int                  fill_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_RINGBUFFER_H_
