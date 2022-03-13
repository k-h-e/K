////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_TEXTWRITER_H_
#define K_CORE_TEXTWRITER_H_

#include <K/Core/TextWriterInterface.h>

namespace K {
namespace Core {

class BlockingOutStreamInterface;

//! Text writer.
class TextWriter : public virtual TextWriterInterface {
  public:
    TextWriter(const std::shared_ptr<BlockingOutStreamInterface> &stream);
    TextWriter()                                   = delete;
    TextWriter(const TextWriter &other)            = delete;
    TextWriter &operator=(const TextWriter &other) = delete;
    TextWriter(TextWriter &&other)                 = delete;
    TextWriter &operator=(TextWriter &&other)      = delete;
    ~TextWriter();

    void Write(const std::string &text) override;
    void WriteLine(const std::string &line) override;
    bool WriteFailed() const override;
    void ClearWriteFailed() override;
    bool ErrorState() const override;
    void SetFinalResultAcceptor(const std::shared_ptr<ResultAcceptor> &resultAcceptor) override;

  private:
    const std::shared_ptr<BlockingOutStreamInterface> stream_;
    std::shared_ptr<ResultAcceptor>                   finalResultAcceptor_;
    bool                                              writeFailed_;
    bool                                              error_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TEXTWRITER_H_
