////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BINARYWRITER_H_
#define K_CORE_BINARYWRITER_H_

#include <memory>
#include <K/Core/BinaryWriterInterface.h>

namespace K {
namespace Core {

class BlockingOutStreamInterface;

//! Binary writer.
class BinaryWriter : public virtual BinaryWriterInterface {
  public:
    BinaryWriter(const std::shared_ptr<BlockingOutStreamInterface> &stream);
    BinaryWriter()                                     = delete;
    BinaryWriter(const BinaryWriter &other)            = delete;
    BinaryWriter &operator=(const BinaryWriter &other) = delete;
    BinaryWriter(BinaryWriter &&other)                 = delete;
    BinaryWriter &operator=(BinaryWriter &&other)      = delete;
    ~BinaryWriter();

    void WriteItem(const void *item, int itemSize) override;
    void Write(int value) override;
    void Write(uint8_t value) override;
    void Write(uint32_t value) override;
    void Write(float value) override;
    void Write(double value) override;
    bool WriteFailed() const override;
    void ClearWriteFailed() override;
    bool ErrorState() const override;
    void SetFinalResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) override;

  private:
    const std::shared_ptr<BlockingOutStreamInterface> stream_;
    std::shared_ptr<Core::ResultAcceptor>             finalResultAcceptor_;
    bool                                              writeFailed_;
    bool                                              error_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BINARYWRITER_H_
