////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/TextWriter.h>

#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/IOOperations.h>
#include <K/Core/Log.h>
#include <K/Core/ResultAcceptor.h>

using std::shared_ptr;
using std::string;
using K::Core::Log;

namespace K {
namespace Core {

TextWriter::TextWriter(const shared_ptr<BlockingOutStreamInterface> &stream)
        : stream_(stream),
          writeFailed_(false),
          error_(false) {
    // Nop.
}

TextWriter::~TextWriter() {
    if (finalResultAcceptor_) {
        if (error_ || writeFailed_) {
            finalResultAcceptor_->OnFailure();
        } else {
            finalResultAcceptor_->OnSuccess();
        }
    }

    if (error_ || writeFailed_) {
        Log::Print(Log::Level::Error, this, []{ return "error while closing"; });
    } else {
        Log::Print(Log::Level::Debug, this, []{ return "closed"; });
    }
}

void TextWriter::Write(const string &text) {
    if (!writeFailed_) {
        if (error_) {
            writeFailed_ = true;
        } else {
            int size = static_cast<int>(text.size() * sizeof(string::value_type));
            if (size > 0) {
                if (!Core::WriteItem(stream_.get(), &text[0], size)) {
                    writeFailed_ = true;
                    error_       = true;
                }
            }
        }
    }
}

void TextWriter::WriteLine(const string &line) {
    Write(line);
    Write("\r\n");
}

bool TextWriter::WriteFailed() const {
    return writeFailed_;
}

void TextWriter::ClearWriteFailed() {
    writeFailed_ = false;
}

bool TextWriter::ErrorState() const {
    return error_;
}

void TextWriter::SetFinalResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    finalResultAcceptor_ = resultAcceptor;
    stream_->SetFinalResultAcceptor(resultAcceptor);
}

}    // Namespace Core.
}    // Namespace K.
