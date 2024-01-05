/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_TEXTWRITER_H_
#define K_CORE_TEXTWRITER_H_

#include <K/Core/OutStreamInterface.h>

namespace K {
namespace Core {

class BlockingOutStreamInterface;

//! Text writer.
class TextWriter : public virtual OutStreamInterface {
  public:
    TextWriter(const std::shared_ptr<BlockingOutStreamInterface> &stream);
    TextWriter()                                   = delete;
    TextWriter(const TextWriter &other)            = delete;
    TextWriter &operator=(const TextWriter &other) = delete;
    TextWriter(TextWriter &&other)                 = delete;
    TextWriter &operator=(TextWriter &&other)      = delete;
    ~TextWriter();

    TextWriter &operator<<(const std::string &text);
    //! Writes the specified string.
    /*!
     *  In case the write operation fails, write failed state will be set
     *  (see <c>WriteFailedInterface::WriteFailed()</c>). If write failed state has already been set when the method
     *  gets called, the write will not execute and the method returns immediately.
     */
    void Write(const std::string &text);
    //! Writes the specified line of text.
    /*!
     *  In case the write operation fails, write failed state will be set
     *  (see <c>WriteFailedInterface::WriteFailed()</c>). If write failed state has already been set when the method
     *  gets called, the write will not execute and the method returns immediately.
     */
    void WriteLine(const std::string &line);

    bool ErrorState() const override;
    std::optional<Error> StreamError() const override;
    void SetCloseResultAcceptor(const std::shared_ptr<ResultAcceptor> &resultAcceptor) override;

  private:
    const std::shared_ptr<BlockingOutStreamInterface> stream_;
    std::shared_ptr<ResultAcceptor>                   closeResultAcceptor_;
    std::optional<Error>                              error_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TEXTWRITER_H_
