////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_TEXTREADER_H_
#define K_CORE_TEXTREADER_H_

#include <memory>
#include <K/Core/TextReaderInterface.h>

namespace K {
namespace Core {

class SeekableBlockingInStreamInterface;

//! Text reader.
class TextReader : public virtual TextReaderInterface {
  public:
    TextReader(const std::shared_ptr<SeekableBlockingInStreamInterface> &stream);
    TextReader()                                   = delete;
    TextReader(const TextReader &other)            = delete;
    TextReader &operator=(const TextReader &other) = delete;
    TextReader(TextReader &&other)                 = delete;
    TextReader &operator=(TextReader &&other)      = delete;
    ~TextReader()                                  = default;

    void Read(char delimiter, std::string *outString) override;
    void Read(const std::string &validCharacters, bool readOther, std::string *outString) override;
    void Skip(const std::string &charactersToSkip, bool skipOther) override;
    bool ReadFailed() const override;
    void ClearReadFailed() override;
    bool Eof() const override;
    bool ErrorState() const override;

  private:
    const std::shared_ptr<SeekableBlockingInStreamInterface> stream_;
    bool                                                     readFailed_;
    bool                                                     eof_;
    bool                                                     error_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TEXTREADER_H_
