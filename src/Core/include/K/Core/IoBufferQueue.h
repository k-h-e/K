/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_IOBUFFERQUEUE_H_
#define K_CORE_IOBUFFERQUEUE_H_

#include <deque>

#include <K/Core/UniqueHandle.h>

namespace K {
    namespace Core {
        class IoBufferInterface;
        class IoBuffers;
        class NonBlockingOutStreamInterface;
    }
}

namespace K {
namespace Core {

//! I/O buffer queue.
class IoBufferQueue {
  public:
    IoBufferQueue();
    IoBufferQueue(const IoBufferQueue &other)            = delete;
    IoBufferQueue &operator=(const IoBufferQueue &other) = delete;
    IoBufferQueue(IoBufferQueue &&other)                 = delete;
    IoBufferQueue &operator=(IoBufferQueue &&other)      = delete;
    ~IoBufferQueue()                                     = default;

    //! Tells the number of payload bytes queued up in the queue.
    std::size_t PayloadSize() const;
    //! Tells whether the queue is empty.
    bool Empty() const;
    //! Clears the queue.
    void Clear();
    //! Removes and returns the next I/O buffer from the queue, or returns a null-handle in case the queue was empty.
    UniqueHandle<IoBufferInterface> Get(); 
    //! Puts the specified I/O buffer back into the queue. 
    /*!
     *  Intended for data that has been previously taken out of the queue via <c>Get()</c>, but could not be consumed
     *  by the client. The put back data will be the first to be delivered with the next call to <c>Get()</c>.
     */
    void PutBack(UniqueHandle<IoBufferInterface> buffer);
    //! Puts the specified I/O buffer into the queue. 
    void Put(UniqueHandle<IoBufferInterface> buffer);
    //! Removes and returns the last I/O buffer put into the queue via Put(), or returns a null-handle in case the queue
    //! was empty.
    UniqueHandle<IoBufferInterface> GetBack(); 

  private:
    std::deque<UniqueHandle<IoBufferInterface>> buffers_;
    std::size_t                                 numBytesTotal_;
};

//! Transfers data from source to destination queue, as long as the destination queue does not violate the specified
//! payload size constraint (and the source queue is not empty).
void Transfer(IoBufferQueue &source, IoBufferQueue &destination, std::size_t maxPayloadSize);
//! Transfers data from the source queue to the specified stream, as long as the stream accepts data (and the source
//! queue is not empty).
void Transfer(IoBufferQueue &source, NonBlockingOutStreamInterface &stream);
//! Puts (a copy of) the specified binary source data to the destination queue.
void Put(const void *sourceData, int sourceDataSize, IoBufferQueue &destination, IoBuffers &ioBuffers);

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_IOBUFFERQUEUE_H_
