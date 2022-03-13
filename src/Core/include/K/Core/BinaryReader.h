////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BINARYREADER_H_
#define K_CORE_BINARYREADER_H_

#include <memory>
#include <K/Core/BinaryReaderInterface.h>

namespace K {
namespace Core {

class BlockingInStreamInterface;

//! Binary reader.
class BinaryReader : public virtual BinaryReaderInterface {
  public:
    BinaryReader(const std::shared_ptr<BlockingInStreamInterface> &stream);
    BinaryReader()                                     = delete;
    BinaryReader(const BinaryReader &other)            = delete;
    BinaryReader &operator=(const BinaryReader &other) = delete;
    BinaryReader(BinaryReader &&other)                 = delete;
    BinaryReader &operator=(BinaryReader &&other)      = delete;
    ~BinaryReader()                                    = default;

    void ReadItem(void *item, int itemSize) override;
    int ReadInt() override;
    uint8_t ReadUInt8() override;
    uint32_t ReadUInt32() override;
    float ReadFloat() override;
    double ReadDouble() override;
    bool ReadFailed() const override;
    void ClearReadFailed() override;
    bool Eof() const override;
    bool ErrorState() const override;

  private:
    const std::shared_ptr<BlockingInStreamInterface> stream_;
    bool                                             readFailed_;
    bool                                             eof_;
    bool                                             error_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BINARYREADER_H_
