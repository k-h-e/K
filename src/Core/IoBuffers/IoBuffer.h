/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BUFFERS_GROUP_IOBUFFER_H_
#define K_CORE_BUFFERS_GROUP_IOBUFFER_H_

#include <K/Core/IoBuffers.h>

namespace K {
namespace Core {

class IoBuffers::Group::IoBuffer : public virtual IoBufferInterface,
                                   public virtual ReferenceCountTrackerInterface {
  public:
    IoBuffer(uint8_t *memory, Group *group, int infoId);
    IoBuffer();
    IoBuffer(const IoBuffer &other)            = delete;
    IoBuffer &operator=(const IoBuffer &other) = delete;
    IoBuffer(IoBuffer &&other);
    IoBuffer &operator=(IoBuffer &&other);
    ~IoBuffer()                                = default;

    void SetSize(int size);

    // IoBufferInterface...
    void *Content() override;
    virtual int Size() const override;

    // ReferenceCountTrackerInterface...
    void OnReferenceAdded() override;
    void OnReferenceRemoved() override;

  private:
    Group   *group_;           // Might be nullptr.
    int     infoId_;
    uint8_t *memory_;          // Might be nullptr.
    int     size_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BUFFERS_GROUP_IOBUFFER_H_
