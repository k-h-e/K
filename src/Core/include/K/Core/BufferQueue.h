/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BUFFERQUEUE_H_
#define K_CORE_BUFFERQUEUE_H_

#include <deque>

#include <K/Core/UniqueHandle.h>

namespace K {
    namespace Core {
        class BufferProviderInterface;
        class ReadableByteSpanInterface;
        class NonBlockingOutStreamInterface;
    }
}

namespace K {
namespace Core {

//! Buffer queue.
class BufferQueue {
  public:
    BufferQueue();
    BufferQueue(const BufferQueue &other)            = delete;
    BufferQueue &operator=(const BufferQueue &other) = delete;
    BufferQueue(BufferQueue &&other)                 = delete;
    BufferQueue &operator=(BufferQueue &&other)      = delete;
    ~BufferQueue()                                   = default;

    //! Tells the number of payload bytes queued up in the queue.
    std::size_t PayloadSize() const;
    //! Tells whether the queue is empty.
    bool Empty() const;
    //! Clears the queue.
    void Clear();
    //! Removes and returns the next buffer from the queue, or returns a null-handle in case the queue was empty.
    UniqueHandle<ReadableByteSpanInterface> Get(); 
    //! Puts the specified buffer back into the queue. 
    /*!
     *  Intended for data that has been previously taken out of the queue via <c>Get()</c>, but could not be consumed
     *  by the client. The put back data will be the first to be delivered with the next call to <c>Get()</c>.
     */
    void PutBack(UniqueHandle<ReadableByteSpanInterface> buffer);
    //! Puts the specified buffer into the queue. 
    void Put(UniqueHandle<ReadableByteSpanInterface> buffer);
    //! Removes and returns the last buffer put into the queue via Put(), or returns a null-handle in case the queue
    //! was empty.
    UniqueHandle<ReadableByteSpanInterface> GetBack(); 

  private:
    std::deque<UniqueHandle<ReadableByteSpanInterface>> buffers_;
    std::size_t                                         numBytesTotal_;
};

//! Transfers data from source to destination queue, as long as the destination queue does not violate the specified
//! payload size constraint (and the source queue is not empty).
void Transfer(BufferQueue &source, BufferQueue &destination, std::size_t maxPayloadSize);
//! Transfers data from the source queue to the specified stream, as long as the stream accepts data (and the source
//! queue is not empty).
void Transfer(BufferQueue &source, NonBlockingOutStreamInterface &stream);
//! Puts (a copy of) the specified binary source data to the destination queue.
void Put(const void *sourceData, int sourceDataSize, BufferQueue &destination, BufferProviderInterface &bufferProvider);

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BUFFERQUEUE_H_
