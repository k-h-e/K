/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_BUFFERS_GROUP_IOBUFFER_H_
#define K_IO_BUFFERS_GROUP_IOBUFFER_H_

#include <K/Core/ByteSpanInterface.h>
#include <K/IO/IoBuffers.h>

namespace K {
namespace IO {

class IoBuffers::Group::IoBuffer : public virtual Core::ByteSpanInterface,
                                   public virtual Core::ReferenceCountTrackerInterface {
  public:
    IoBuffer(uint8_t *memory, Group *group, int infoId);
    IoBuffer();
    IoBuffer(const IoBuffer &other)            = delete;
    IoBuffer &operator=(const IoBuffer &other) = delete;
    IoBuffer(IoBuffer &&other);
    IoBuffer &operator=(IoBuffer &&other);
    ~IoBuffer()                                = default;

    void SetSize(int size);

    // ByteSpanInterface...
    const void *ByteSpanStartReadOnly() override;
    virtual int ByteSpanSize() const override;
    void *ByteSpanStart() override;

    // ReferenceCountTrackerInterface...
    void OnReferenceAdded() override;
    void OnReferenceRemoved() override;

  private:
    Group   *group_;           // Might be nullptr.
    int     infoId_;
    uint8_t *memory_;          // Might be nullptr.
    int     size_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFERS_GROUP_IOBUFFER_H_
