/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_BUFFEREDCONNECTION_SHAREDSTATE_H_
#define K_IO_BUFFEREDCONNECTION_SHAREDSTATE_H_

#include <memory>
#include <mutex>
#include <condition_variable>
#include <K/Core/RingBuffer.h>
#include <K/IO/ConnectionIO.h>
#include <K/IO/Deprecated/BufferedConnection.h>

namespace K {
    namespace Core {
        class RawStreamHandlerInterface;
    }
}

namespace K {
namespace IO {
namespace Deprecated {

//! State shared between threads of the buffered connection.
class BufferedConnection::SharedState : public virtual ConnectionIO::ClientInterface {
  public:
    SharedState(int bufferSizeThreshold, const std::shared_ptr<ConnectionIO> &connectionIO);
    SharedState(const SharedState &other)             = delete;
    SharedState &operator=(const SharedState &other)  = delete;
    SharedState(const SharedState &&other)            = delete;
    SharedState &operator=(const SharedState &&other) = delete;

    void SetError();
    bool Register(const std::shared_ptr<Core::RawStreamHandlerInterface> &handler);
    void Unregister(const std::shared_ptr<Core::RawStreamHandlerInterface> &handler);
    void WriteItem(const void *item, int itemSize);
    bool WriteFailed();
    void ClearWriteFailed();
    bool Error();
    std::optional<StreamInterface::Error> StreamError();

    // ConnectionIO::ClientInterface...
    bool OnDataRead(const void *data, int dataSize) override;
    int OnReadyWrite(void *buffer, int bufferSize) override;
    void OnIncompleteWrite(const void *unwrittenData, int unwrittenDataSize) override;
    void OnCustomCall() override;
    void OnEof() override;
    void OnError() override;

  private:
    // Expects lock to be held.
    void EnsureHandlerCalledInitially();

    std::mutex                                       lock_;    // Protects everything below...

    std::condition_variable                          writeCanContinue_;
    std::shared_ptr<ConnectionIO>                    connectionIO_;
    std::shared_ptr<Core::RawStreamHandlerInterface> handler_;
    bool                                             handlerCalledInitially_;
    Core::RingBuffer                                 writeBuffer_;
    int                                              bufferSizeThreshold_;
    bool                                             canNotWrite_;
    std::optional<StreamInterface::Error>            error_;
};

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFEREDCONNECTION_SHAREDSTATE_H_
