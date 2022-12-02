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

#include <cassert>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/Log.h>
#include <K/Core/ResultAcceptor.h>

using std::shared_ptr;
using std::string;
using K::Core::Log;

namespace K {
namespace Core {

TextWriter::TextWriter(const shared_ptr<BlockingOutStreamInterface> &stream)
        : stream_(stream),
          error_(Error::None) {
    // Nop.
}

TextWriter::~TextWriter() {
    if (closeResultAcceptor_) {
        if (error_ != Error::None) {
            closeResultAcceptor_->OnFailure();
        } else {
            closeResultAcceptor_->OnSuccess();
        }
    }
}

TextWriter &TextWriter::operator<<(const std::string &text) {
    Write(text);
    return *this;
}

void TextWriter::Write(const string &text) {
    if (!ErrorState()) {
        int size = static_cast<int>(text.size() * sizeof(string::value_type));
        if (size > 0) {
            Core::WriteItem(stream_.get(), &text[0], size);
            if (stream_->ErrorState()) {
                Error error = stream_->StreamError();
                assert (error != Error::None);
                error_ = error;
            }
        }
    }
}

void TextWriter::WriteLine(const string &line) {
    Write(line);
    Write("\r\n");
}

bool TextWriter::ErrorState() const {
    return (error_ != Error::None);
}

StreamInterface::Error TextWriter::StreamError() const {
    return error_;
}

void TextWriter::SetCloseResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    closeResultAcceptor_ = resultAcceptor;
    stream_->SetCloseResultAcceptor(resultAcceptor);
}

}    // Namespace Core.
}    // Namespace K.
