/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_CONNECTION_SYNCHRONIZEDSTATE_H_
#define K_IO_CONNECTION_SYNCHRONIZEDSTATE_H_

#include <mutex>
#include <optional>
#include <K/Core/IoBufferQueue.h>
#include <K/IO/ConnectionIO.h>
#include <K/IO/Connection.h>

namespace K {
namespace IO {

//! Connection state shared between threads.
/*!
 *  The class is thread safe (all public instance methods).
 */
class Connection::SynchronizedState : public virtual IO::ConnectionIO::ClientInterface {
  public:
    SynchronizedState(const std::shared_ptr<Core::RunLoop> &runLoop, int bufferSizeConstraint);
    SynchronizedState()                                          = delete;
    SynchronizedState(const SynchronizedState &other)            = delete;
    SynchronizedState &operator=(const SynchronizedState &other) = delete;
    SynchronizedState(SynchronizedState &&other)                 = delete;
    SynchronizedState &operator=(SynchronizedState &&other)      = delete;
    ~SynchronizedState()                                         = default;

    void SetRunLoopClientId(int id);
    void Sync(LoopThreadState *loopThreadState);

    bool OnDataRead(Core::UniqueHandle<Core::IoBufferInterface> buffer) override;
    Core::UniqueHandle<Core::IoBufferInterface> OnReadyWrite() override;
    void OnIncompleteWrite(Core::UniqueHandle<Core::IoBufferInterface> buffer) override;
    void OnCustomCall() override;
    void OnEof() override;
    void OnError() override;

  private:
    // Expects lock to be held.
    void RequestSync(std::unique_lock<std::mutex> &critical);

    std::mutex lock_;    // Protects everything below...
    const std::shared_ptr<Core::RunLoop> runLoop_;
    std::optional<int>                   runLoopClientId_;
    Core::IoBufferQueue                  readQueue_;
    Core::IoBufferQueue                  writeQueue_;
    bool                                 error_;
    bool                                 eof_;
    bool                                 ioReadPaused_;
    bool                                 ioWritePaused_;
    int                                  bufferSizeConstraint_;
    bool                                 syncRequested_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONNECTION_SYNCHRONIZEDSTATE_H_
