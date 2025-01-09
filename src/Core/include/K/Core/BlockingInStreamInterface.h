/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BLOCKINGINSTREAMINTERFACE_H_
#define K_CORE_BLOCKINGINSTREAMINTERFACE_H_

#include <string>

#include <K/Core/InStreamInterface.h>

namespace K {
namespace Core {

//! Interface to blocking input streams.
class BlockingInStreamInterface : public virtual InStreamInterface {
  public:
    //! Reads data into the specified buffer with blocking semantics.
    /*!
     *  \return
     *  Number of bytes actually read. Will not be larger than the buffer size. Might be less than the buffer size
     *  (short read). <c>0</c> means that the read operation has failed and that the stream will be in
     *  <c>ErrorState()</c>.
     */
    virtual int ReadBlocking(void *buffer, int bufferSize) = 0;
};

//! Reads a binary item of specified size (in bytes).
void ReadItem(BlockingInStreamInterface &stream, void *item, int itemSize);

//! Skips the next <c>numBytes</c> bytes.
void Skip(BlockingInStreamInterface &stream, int numBytes);

template<typename T>
BlockingInStreamInterface &operator>>(BlockingInStreamInterface &stream, T &outValue) {
    ReadItem(stream, &outValue, sizeof(T));
    return stream;
}

BlockingInStreamInterface &operator>>(BlockingInStreamInterface &stream, std::string &outValue);

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGINSTREAMINTERFACE_H_
