/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BLOCKINGOUTSTREAMINTERFACE_H_
#define K_CORE_BLOCKINGOUTSTREAMINTERFACE_H_

#include <K/Core/OutStreamInterface.h>

namespace K {
namespace Core {

//! Interface to blocking output streams.
class BlockingOutStreamInterface : public virtual OutStreamInterface {
  public:
    //! Writes the specified data to the stream with blocking semantics.
    /*!
     *  \return
     *  Number of bytes actually written. Might be less than the data size (short write). <c>0</c> means that the write
     *  operation has failed and that the stream will be in <c>ErrorState()</c>.
     */
    virtual int WriteBlocking(const void *data, int dataSize) = 0;
};

//! Writes a binary item of specified size (in bytes).
void WriteItem(BlockingOutStreamInterface *stream, const void *item, int itemSize);

template<typename T>
BlockingOutStreamInterface &operator<<(BlockingOutStreamInterface &stream, const T &value) {
    WriteItem(&stream, &value, sizeof(T));
    return stream;
}

BlockingOutStreamInterface &operator<<(BlockingOutStreamInterface &stream, const std::string &value);

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BLOCKINGOUTSTREAMINTERFACE_H_
