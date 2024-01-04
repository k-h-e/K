/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/BitStream.h>

#include <K/Core/Log.h>

using std::to_string;
using K::Core::Log;

namespace K {
namespace Core {

BitStream::BitStream()
        : free_(32) {
    // Nop.
}

void BitStream::AppendBytes(const void *data, int dataSize) {
    const uint8_t *dataPtr = static_cast<const uint8_t *>(data);
    for (int i = 0; i < dataSize; ++i) {
        buffer_.push_back(*dataPtr++);
    }
}

bool BitStream::ReadBits(int numBits, uint32_t *outBits) {
    if ((numBits >= 0) && (numBits <= 24)) {
        while (free_ >= 8) {
            if (buffer_.empty()) {
                break;
            }

            uint32_t value = static_cast<uint32_t>(buffer_.front());
            buffer_.pop_front();
            int numToShift = free_ - 8;
            if (numToShift) {
                value = value << numToShift;
            }
            accumulator_ |= value;

            free_ -= 8;
        }

        if (32 - free_ >= numBits) {
            *outBits     = accumulator_ >> (32 - numBits);
            accumulator_ = accumulator_ << numBits;
            free_ += numBits;
            Log::Print(Log::Level::Debug, this, [&]{ return "delivered " + to_string(numBits) + " bits"; });
            return true;
        }
    }

    return false;
}

}    // Namespace Core.
}    // Namespace K.
