/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_BUFFERS_H_
#define K_CORE_BUFFERS_H_

#include <vector>

#include <K/Core/BufferInterface.h>
#include <K/Core/BufferProviderInterface.h>
#include <K/Core/ReferenceCountTrackerInterface.h>
#include <K/Core/ReusableItems.h>
#include <K/Core/UniqueHandle.h>

namespace K {
namespace Core {

//! Provides re-usable buffers for temporary storage of binary data.
/*!
 *  The class is thread-safe, i.e its public non-static member methods are. The handed out buffers may be passed to
 *  different, arbitrary threads.
 */
class Buffers : public virtual BufferProviderInterface {
  public:
    Buffers();
    Buffers(const Buffers &other)            = delete;
    Buffers &operator=(const Buffers &other) = delete;
    Buffers(Buffers &&other)                 = delete;
    Buffers &operator=(Buffers &&other)      = delete;
    ~Buffers()                               = default;

    // BufferProviderInterface...
    UniqueHandle<BufferInterface> Get(int size) override;

  private:
    struct State;
    
    class Group : public virtual Interface {
      public:
        static const int idleBuffers  = 0;
        static const int inUseBuffers = 1;

        class Buffer;

        struct BufferInfo {
            Buffer *buffer;
            int    numReferences;

            BufferInfo() : buffer{nullptr}, numReferences{0} {}
        };

        Group(int bufferSize, int buffersPerBucket, State &state);
        Group()                              = delete;
        Group(const Group &other)            = delete;
        Group &operator=(const Group &other) = delete;
        Group(Group &&other)                 = delete;
        Group &operator=(Group &&other)      = delete;
        ~Group();
        
        int BufferSize() const;
        Buffer *Get();
        
      private:
        void AddBucket();

        State                     &state_;
        int                       bufferSize_;
        int                       buffersPerBucket_;
        int                       alignment_;
        int                       spacing_;
        ReusableItems<BufferInfo> bufferInfos_;
        std::vector<Buffer *>     bufferBuckets_;
        std::vector<uint8_t *>    memoryBuckets_;
    };

    struct State : public virtual Interface {
        std::mutex                          mutex;
        std::vector<std::unique_ptr<Group>> groups;
        int                                 numBuffersTotal { 0 };
        int                                 numBuffersInUse { 0 };
        uintptr_t                           numBytesTotal   { 0u };
        uintptr_t                           numBytesInUse   { 0u };

        State()                              = default;
        State(const State &other)            = delete;
        State &operator=(const State &other) = delete;
        State(State &&other)                 = delete;
        State &operator=(State &&other)      = delete;
        ~State()                             = default;
    };

    Group *SelectGroup(int size);

    std::shared_ptr<State> state_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BUFFERS_H_
