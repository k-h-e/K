////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/BinaryWriter.h>

#include <cassert>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/IOOperations.h>
#include <K/Core/Log.h>
#include <K/Core/ResultAcceptor.h>

using std::shared_ptr;

namespace K {
namespace Core {

BinaryWriter::BinaryWriter(const shared_ptr<BlockingOutStreamInterface> &stream)
        : stream_(stream),
          streamDumb_(nullptr),
          writeFailed_(false),
          error_(false) {
    // Nop.
}

BinaryWriter::BinaryWriter(BlockingOutStreamInterface *stream)
        : streamDumb_(stream),
          writeFailed_(false),
          error_(false) {
    assert (stream);
}

BinaryWriter::~BinaryWriter() {
    if (finalResultAcceptor_) {
        if (error_ || writeFailed_) {
            finalResultAcceptor_->OnFailure();
        } else {
            finalResultAcceptor_->OnSuccess();
        }
    }

    if (error_ || writeFailed_) {
        Log::Print(Log::Level::Error, this, []{ return "error while closing"; });
    }
}

void BinaryWriter::Reset() {
    writeFailed_ = false;
    error_       = false;
}

void BinaryWriter::Reset(BlockingOutStreamInterface *stream) {
    assert (stream);
    if (!streamDumb_) {
        stream_.reset();
    }
    streamDumb_  = stream;

    writeFailed_ = false;
    error_       = false;
}

void BinaryWriter::WriteItem(const void *item, int itemSize) {
    if (!writeFailed_) {
        if (error_) {
            writeFailed_ = true;
        } else {
            if (!Core::WriteItem(streamDumb_ ? streamDumb_ : stream_.get(), item, itemSize)) {
                writeFailed_ = true;
                error_       = true;
            }
        }
    }
}

void BinaryWriter::Write(int value) {
    WriteItem(&value, static_cast<int>(sizeof(value)));
}

void BinaryWriter::Write(uint8_t value) {
    WriteItem(&value, static_cast<int>(sizeof(value)));
}

void BinaryWriter::Write(uint32_t value) {
    WriteItem(&value, static_cast<int>(sizeof(value)));
}

void BinaryWriter::Write(float value) {
    WriteItem(&value, static_cast<int>(sizeof(value)));
}

void BinaryWriter::Write(double value) {
    WriteItem(&value, static_cast<int>(sizeof(value)));
}

bool BinaryWriter::WriteFailed() const {
    return writeFailed_;
}

void BinaryWriter::ClearWriteFailed() {
    writeFailed_ = false;
}

bool BinaryWriter::ErrorState() const {
    return error_;
}

void BinaryWriter::SetFinalResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    finalResultAcceptor_ = resultAcceptor;
    (streamDumb_ ? streamDumb_ : stream_.get())->SetFinalResultAcceptor(resultAcceptor);
}

}    // Namespace Core.
}    // Namespace K.
