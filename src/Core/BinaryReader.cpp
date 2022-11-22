////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/BinaryReader.h>

#include <cassert>
#include <K/Core/BlockingInStreamInterface.h>

using std::shared_ptr;

namespace K {
namespace Core {

BinaryReader::BinaryReader(const shared_ptr<BlockingInStreamInterface> &stream)
        : stream_(stream),
          streamDumb_(nullptr),
          readFailed_(false),
          eof_(false),
          error_(false) {
    // Nop.
}

BinaryReader::BinaryReader(BlockingInStreamInterface *stream)
        : streamDumb_(stream),
          readFailed_(false),
          eof_(false),
          error_(false) {
    // Nop.
}

void BinaryReader::Reset() {
    readFailed_ = false;
    eof_        = false;
    error_      = false;
}

void BinaryReader::Reset(BlockingInStreamInterface *stream) {
    assert (stream);
    if (!streamDumb_) {
        stream_.reset();
    }
    streamDumb_  = stream;

    readFailed_ = false;
    eof_        = false;
    error_      = false;
}

void BinaryReader::ReadItem(void *item, int itemSize) {
    assert(itemSize > 0);
    if (!readFailed_) {
        if (error_ || eof_) {
            readFailed_ = true;
        } else {
            uint8_t *dest   = static_cast<uint8_t *>(item);
            int     numLeft = itemSize;
            while (numLeft) {
                int numRead = (streamDumb_ ? streamDumb_ : stream_.get())->ReadBlocking(dest, numLeft);
                if (numRead) {
                    dest    += numRead;
                    numLeft -= numRead;
                } else {
                    readFailed_ = true;
                    if ((streamDumb_ ? streamDumb_ : stream_.get())->Eof()) {
                        eof_ = true;
                    } else {
                        error_ = true;
                    }
                    return;
                }
            }
        }
    }
}

int BinaryReader::ReadInt() {
    int value;
    ReadItem(&value, static_cast<int>(sizeof(value)));
    return value;
}

uint8_t BinaryReader::ReadUInt8() {
    uint8_t value;
    ReadItem(&value, static_cast<int>(sizeof(value)));
    return value;
}

uint32_t BinaryReader::ReadUInt32() {
    uint32_t value;
    ReadItem(&value, static_cast<int>(sizeof(value)));
    return value;
}

float BinaryReader::ReadFloat() {
    float value;
    ReadItem(&value, static_cast<int>(sizeof(value)));
    return value;
}

double BinaryReader::ReadDouble() {
    double value;
    ReadItem(&value, static_cast<int>(sizeof(value)));
    return value;
}

bool BinaryReader::ReadFailed() const {
    return readFailed_;
}

void BinaryReader::ClearReadFailed() {
    readFailed_ = false;
}

bool BinaryReader::Eof() const {
    return (eof_ && !error_);
}

bool BinaryReader::ErrorState() const {
    return error_;
}

}    // Namespace Core.
}    // Namespace K.
