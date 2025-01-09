/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BINARYQUEUE_H_
#define K_CORE_BINARYQUEUE_H_

#include <cstdint>
#include <vector>

namespace K {
namespace Core {

class NonBlockingOutStreamInterface;

//! Binary data queue.
class BinaryQueue {
  public:
    BinaryQueue();
    BinaryQueue(const BinaryQueue &other)            = delete;
    BinaryQueue &operator=(const BinaryQueue &other) = delete;
    BinaryQueue(BinaryQueue &&other)                 = delete;
    BinaryQueue &operator=(BinaryQueue &&other)      = delete;
    ~BinaryQueue()                                   = default;

    //! Returns the number of bytes currently in the queue.
    int Size() const;
    //! Tells whether the queue is currently empty.
    bool Empty() const;
    //! Gets (and removes) at most <c>bufferSize</c> bytes from the queue.
    /*!
     *  \return
     *  Number of bytes actually fetched from the queue. <c>0</c> means the queue was empty.
     */
    int Get(void *outBuffer, int bufferSize);
    //! Puts (a copy of) the specified data in the queue.
    /*!
     *  The queue grows as needed.
     */
    void Put(const void *data, int dataSize);
    //! Puts back (a copy of) the specified data into the queue.
    /*!
     *  Intended for data that has been previously taken out of the queue via <c>Get()</c>, but could not be consumed
     *  by the client. The put back data will be the first to be delivered with the next call to <c>Get()</c>.
     *
     *  The queue grows as needed.
     */
    void PutBack(const void *data, int dataSize);
    //! Transfers data to the specified other queue, as long as the other queue's size does not exceed the size
    //! specified.
    void TransferTo(BinaryQueue &other, int maxSize);

  private:
    void Grow();

    std::vector<uint8_t> buffer_;
    int                  capacity_;
    int                  cursor_;
    int                  fill_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BINARYQUEUE_H_
