/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_SUBSTREAM_H_
#define K_IO_SUBSTREAM_H_

#include <cstdint>
#include <memory>
#include <K/Core/SeekableBlockingInStreamInterface.h>

namespace K {
namespace IO {

//! Stream decorator, granting read access only to a substream.
class SubStream : public virtual Core::SeekableBlockingInStreamInterface {
  public:
    SubStream(const std::shared_ptr<Core::SeekableBlockingInStreamInterface> &stream, int64_t offset, int64_t size);
    SubStream(const SubStream &other)             = delete;
    SubStream &operator=(const SubStream &other)  = delete;
    SubStream(const SubStream &&other)            = delete;
    SubStream &operator=(const SubStream &&other) = delete;
    ~SubStream()                                  = default;

    int ReadBlocking(void *buffer, int bufferSize) override;
    void Seek(int64_t position) override;
    void RecoverAndSeek(int64_t position) override;
    int64_t StreamPosition() const override;
    bool ErrorState() const override;
    std::optional<Error> StreamError() const override;

  private:
    std::shared_ptr<Core::SeekableBlockingInStreamInterface> stream_;
    int64_t                                                  offset_;
    int64_t                                                  size_;
    int64_t                                                  position_;
    std::optional<Core::StreamInterface::Error>              error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_SUBSTREAM_H_
