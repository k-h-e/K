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
          readFailed_(false),
          eof_(false),
          error_(false) {
    // Nop.
}

void TextReader::Read(char delimiter, string *outString) {
    outString->clear();
    if (!readFailed_) {
        if (error_ || eof_) {
            readFailed_ = true;
        } else {
            char character;
            bool success = false;
            while (!success && !readFailed_) {
                if (Core::ReadItem(stream_.get(), &character, static_cast<int>(sizeof(character)))) {
                    if (character == delimiter) {
                        success = true;
                    } else {
                        outString->push_back(character);
                    }
                } else {
                    if (stream_->Eof()) {
                        eof_    = true;
                        success = true;
                    } else {
                        readFailed_ = true;
                        error_      = true;
                    }
                }
            }
        }
    }
}

void TextReader::Read(const string &validCharacters, bool readOther, string *outString) {
    outString->clear();
    if (!readFailed_) {
        if (error_ || eof_) {
            readFailed_ = true;
        } else {
            char character;
            bool success = false;
            while (!success && !readFailed_) {
                int64_t position = stream_->StreamPosition();
                if (Core::ReadItem(stream_.get(), &character, static_cast<int>(sizeof(character)))) {
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
                    if (stream_->Eof()) {
                        eof_    = true;
                        success = true;
                    } else {
                        readFailed_ = true;
                        error_      = true;
                    }
                }
            }
        }
    }
}

void TextReader::Skip(const string &charactersToSkip, bool skipOther) {
    if (!readFailed_) {
        if (error_ || eof_) {
            readFailed_ = true;
        } else {
            char character;
            bool success = false;
            while (!success && !readFailed_) {
                int64_t position = stream_->StreamPosition();
                if (Core::ReadItem(stream_.get(), &character, static_cast<int>(sizeof(character)))) {
                    bool skip = (charactersToSkip.find(character) != string::npos);
                    if (skipOther) {
                        skip = !skip;
                    }

                    if (!skip) {
                        stream_->Seek(position);
                        success = true;
                    }
                } else {
                    if (stream_->Eof()) {
                        eof_    = true;
                        success = true;
                    } else {
                        readFailed_ = true;
                        error_      = true;
                    }
                }
            }
        }
    }
}

bool TextReader::ReadFailed() const {
    return readFailed_;
}

void TextReader::ClearReadFailed() {
    readFailed_ = false;
}

bool TextReader::Eof() const {
    return (eof_ && !error_);
}

bool TextReader::ErrorState() const {
    return error_;
}

}    // Namespace Core.
}    // Namespace K.
