/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BITSTREAM_H_
#define K_CORE_BITSTREAM_H_

#include <deque>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Bit stream.
class BitStream : public virtual Interface {
  public:
    BitStream();
    BitStream(const BitStream &other)            = delete;
    BitStream &operator=(const BitStream &other) = delete;
    BitStream(BitStream &&other)                 = delete;
    BitStream &operator=(BitStream &&other)      = delete;

    //! Appends the specified bytes to the stream's buffer.
    void AppendBytes(const void *data, int dataSize);
    //! Reads the next <c>numBits <= 24</c> bits and returns them as the lower bits of a <c>uint32_t</c>.
    /*!
     *  \return <c>false</c> in case there are currently not enough bits left in the stream. The stream will then be
     *          left unchanged, and the output parameter will be undefined.
     */
    bool ReadBits(int numBits, uint32_t *outBits);

  private:
    std::deque<uint8_t> buffer_;
    uint32_t            accumulator_;
    int                 free_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BITSTREAM_H_
