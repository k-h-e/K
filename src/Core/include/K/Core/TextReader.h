/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_TEXTREADER_H_
#define K_CORE_TEXTREADER_H_

#include <memory>
#include <string>

#include <K/Core/InStreamInterface.h>

namespace K {
    namespace Core {
        class SeekableBlockingInStreamInterface;
    }
}

namespace K {
namespace Core {

//! Text reader.
class TextReader : public virtual InStreamInterface {
  public:
    TextReader(const std::shared_ptr<SeekableBlockingInStreamInterface> &stream);
    TextReader()                                   = delete;
    TextReader(const TextReader &other)            = delete;
    TextReader &operator=(const TextReader &other) = delete;
    TextReader(TextReader &&other)                 = delete;
    TextReader &operator=(TextReader &&other)      = delete;
    ~TextReader()                                  = default;

    //! Reads a string.
    /*!
     *  Encountering the specified delimiter or EOF will properly terminate reading. The next read will happen after the
     *  delimiter, or at EOF respectively.
     */
    void Read(char delimiter, std::string &outString);
    //! Reads a string.
    /*!
     *  Encountering a character not in the specified valid set or EOF will properly terminate reading. The next read
     *  will happen at the offending position.
     *
     *  \param readOther
     *  If set, the complement of the specified set will be treated as the valid characters.
     */
    void Read(const std::string &validCharacters, bool readOther, std::string &outString);
    //! Skips the specified characters.
    /*!
     *  Encountering a character not in the specified skip set or EOF will properly terminate skipping. The next read
     *  will happen at the offending position.
     *
     *  \param skipOther
     *  If set, the complement of the specified set will be treated as the characters to skip.
     */
    void Skip(const std::string &charactersToSkip, bool skipOther);

    bool ErrorState() const override;
    std::optional<Error> StreamError() const override;

  private:
    const std::shared_ptr<SeekableBlockingInStreamInterface> stream_;
      std::optional<Error>                                   error_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TEXTREADER_H_
