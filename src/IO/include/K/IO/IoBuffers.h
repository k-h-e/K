/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_IOBUFFERS_H_
#define K_IO_IOBUFFERS_H_

#include <mutex>
#include <vector>
#include <mutex>

#include <K/Core/BufferProviderInterface.h>
#include <K/Core/ReferenceCountTrackerInterface.h>
#include <K/Core/ReusableItems.h>
#include <K/Core/UniqueHandle.h>

namespace K {
namespace IO {

//! Provides re-usable buffers for temporary storage of binary data.
/*!
 *  The class is thread-safe, i.e its public non-static member methods are. The handed out buffers may be passed to
 *  different, arbitrary threads.
 */
class IoBuffers : public virtual Core::BufferProviderInterface {
  public:
    IoBuffers();
    IoBuffers(const IoBuffers &other)            = delete;
    IoBuffers &operator=(const IoBuffers &other) = delete;
    IoBuffers(IoBuffers &&other)                 = delete;
    IoBuffers &operator=(IoBuffers &&other)      = delete;
    ~IoBuffers()                                 = default;

    //! Logs current statistics.
    void LogStatistics();

    // BufferProviderInterface...
    Core::UniqueHandle<Core::ByteSpanInterface> Get(int size);

  private:
    struct State;
    
    class Group : public virtual Interface {
      public:
        static const int idleBuffers  = 0;
        static const int inUseBuffers = 1;

        class IoBuffer;

        struct BufferInfo {
            IoBuffer *buffer;
            int      numReferences;

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
        IoBuffer &Get();
        void LogStatistics();
        
      private:
        void AddBucket();

        State                           &state_;
        int                             bufferSize_;
        int                             buffersPerBucket_;
        int                             alignment_;
        int                             spacing_;
        Core::ReusableItems<BufferInfo> bufferInfos_;
        std::vector<IoBuffer *>         bufferBuckets_;
        std::vector<uint8_t *>          memoryBuckets_;
        int                             numBuffers_;
        int                             numBuffersInUse_;
        int                             numBytes_;
        int                             numBytesInUse_;
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

    Group &SelectGroup(int size);

    std::shared_ptr<State> state_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_IOBUFFERS_H_
