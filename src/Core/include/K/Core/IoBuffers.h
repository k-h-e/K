/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_IOBUFFERS_H_
#define K_CORE_IOBUFFERS_H_

#include <vector>

#include <K/Core/IoBufferInterface.h>
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
class IoBuffers : public virtual Interface {
  public:
    IoBuffers();
    IoBuffers(const IoBuffers &other)            = delete;
    IoBuffers &operator=(const IoBuffers &other) = delete;
    IoBuffers(IoBuffers &&other)                 = delete;
    IoBuffers &operator=(IoBuffers &&other)      = delete;
    ~IoBuffers()                                 = default;

    //! Provides an I/O buffer of the size specified, or a smaller one in case the requested size exceeds the maximum
    //! supported buffer size.
    /*!
     *  The provided buffer remains exlusively allocated to the owner of the passed out unique handle for as long as
     *  that handle - or a potential decendant - lives.
     * 
     *  \return I/O buffer.
     */ 
    UniqueHandle<IoBufferInterface> Get(int size);

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
        IoBuffer *Get();
        
      private:
        void AddBucket();

        State                     &state_;
        int                       bufferSize_;
        int                       buffersPerBucket_;
        int                       alignment_;
        int                       spacing_;
        ReusableItems<BufferInfo> bufferInfos_;
        std::vector<IoBuffer *>   bufferBuckets_;
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

#endif    // K_CORE_IOBUFFERS_H_