////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/TextReader.h>

#include <K/Core/SeekableBlockingInStreamInterface.h>
#include <K/Core/IOOperations.h>

using std::shared_ptr;
using std::string;

namespace K {
namespace Core {

TextReader::TextReader(const shared_ptr<SeekableBlockingInStreamInterface> &stream)
        : stream_(stream),
          error_(Error::None) {
    // Nop.
}

void TextReader::Read(char delimiter, string *outString) {
    outString->clear();
    if (!ErrorState()) {
        if (stream_->StreamError() == Error::Eof) {
            error_ = Error::Eof;
        } else {
            char character;
            bool success = false;
            while (!success && !ErrorState()) {
                Core::ReadItem(stream_.get(), &character, static_cast<int>(sizeof(character)));
                if (!stream_->ErrorState()) {
                    if (character == delimiter) {
                        success = true;
                    } else {
                        outString->push_back(character);
                    }
                } else {
                    Error error = stream_->StreamError();
                    assert(error != Error::None);
                    if (error == Error::Eof) {
                        success = true;
                    } else {
                        error_ = error;
                    }
                }
            }
        }
    }
}

void TextReader::Read(const string &validCharacters, bool readOther, string *outString) {
    outString->clear();
    if (!ErrorState()) {
        if (stream_->StreamError() == Error::Eof) {
            error_ = Error::Eof;
        } else {
            char character;
            bool success = false;
            while (!success && !ErrorState()) {
                int64_t position = stream_->StreamPosition();
                Core::ReadItem(stream_.get(), &character, static_cast<int>(sizeof(character)));
                if (!stream_->ErrorState()) {
                    bool characterValid = (validCharacters.find(character) != string::npos);
                    if (readOther) {
                        characterValid = !characterValid;
                    }

                    if (characterValid) {
                        outString->push_back(character);
                    } else {
                        stream_->Seek(position);
                        success = true;
                    }
                } else {
                    Error error = stream_->StreamError();
                    assert(error != Error::None);
                    if (error == Error::Eof) {
                        success = true;
                    } else {
                        error_ = error;
                    }
                }
            }
        }
    }
}

void TextReader::Skip(const string &charactersToSkip, bool skipOther) {
    if (!ErrorState()) {
        if (stream_->StreamError() == Error::Eof) {
            error_ = Error::Eof;
        } else {
            char character;
            bool success = false;
            while (!success && !ErrorState()) {
                int64_t position = stream_->StreamPosition();
                Core::ReadItem(stream_.get(), &character, static_cast<int>(sizeof(character)));
                if (!stream_->ErrorState()) {
                    bool skip = (charactersToSkip.find(character) != string::npos);
                    if (skipOther) {
                        skip = !skip;
                    }

                    if (!skip) {
                        stream_->Seek(position);
                        success = true;
                    }
                } else {
                    Error error = stream_->StreamError();
                    assert(error != Error::None);
                    if (error == Error::Eof) {
                        success = true;
                    } else {
                        error_ = error;
                    }
                }
            }
        }
    }
}

bool TextReader::ErrorState() const {
    return (error_ != Error::None);
}

StreamInterface::Error TextReader::StreamError() const {
    return error_;
}

}    // Namespace Core.
}    // Namespace K.
