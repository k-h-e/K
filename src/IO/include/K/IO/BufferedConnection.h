#ifndef K_IO_BUFFEREDCONNECTION_H_
#define K_IO_BUFFEREDCONNECTION_H_

#include <memory>
#include <K/IO/ConnectionStreamInterface.h>

namespace K {
namespace IO {

class ConnectionIO;
class StreamHandlerInterface;

//! Buffered connection.
class BufferedConnection : public virtual ConnectionStreamInterface {
  public:
    //! The connection takes ownership over the UNIX file descriptor.
    /*!
     *  Bad file descriptors < 0 are accepted, with the connection being initialized to error state right away.
     */
    BufferedConnection(int fd, int bufferSizeThreshold, const std::shared_ptr<Core::Result> &resultAcceptor,
                       const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    BufferedConnection(const BufferedConnection &other)             = delete;
    BufferedConnection &operator=(const BufferedConnection &other)  = delete;
    BufferedConnection(const BufferedConnection &&other)            = delete;
    BufferedConnection &operator=(const BufferedConnection &&other) = delete;
    ~BufferedConnection();

    //! Triggers error state.
    /*!
     *  This method is thread-safe. (Note that the class as a whole is not.)
     *
     *  Error state will hit in a deferred fashion - the object is not guaranteed to already be in error state when the
     *  method returns.
     */
    void TriggerErrorState();
    bool Register(const std::shared_ptr<StreamHandlerInterface> &handler) override;
    void Unregister(const std::shared_ptr<StreamHandlerInterface> &handler) override;
    void WriteItem(const void *item, int itemSize) override;
    bool Good() const override;
    bool Eof() override;
    void ClearEof() override;
    bool ErrorState() override;
    void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) override;

  private:
    class SharedState;

    std::shared_ptr<SharedState> sharedState_;

    std::shared_ptr<K::IO::ConnectionIO> connectionIO_;
    int                                  fd_;
    std::shared_ptr<Core::Result>        finalResultAcceptor_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFEREDCONNECTION_H_
