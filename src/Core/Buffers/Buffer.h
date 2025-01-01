/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BUFFERS_GROUP_BUFFER_H_
#define K_CORE_BUFFERS_GROUP_BUFFER_H_

#include <K/Core/Buffers.h>

namespace K {
namespace Core {

class Buffers::Group::Buffer : public virtual BufferInterface,
                               public virtual ReferenceCountTrackerInterface {
  public:
    Buffer(uint8_t *memory, Group *group, int infoId);
    Buffer();
    Buffer(const Buffer &other)            = delete;
    Buffer &operator=(const Buffer &other) = delete;
    Buffer(Buffer &&other);
    Buffer &operator=(Buffer &&other);
    ~Buffer()                              = default;

    void SetSize(int size);

    // BufferInterface...
    const void *Data() const override;
    virtual int DataSize() const override;

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

#endif    // K_CORE_BUFFERS_GROUP_BUFFER_H_
