/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_DEPRECATED_BUFFEREDCONNECTION_H_
#define K_IO_DEPRECATED_BUFFEREDCONNECTION_H_

#include <memory>
#include <optional>
#include <K/Core/Deprecated/ConnectionStreamInterface.h>

namespace K {
    namespace Core {
        class RawStreamHandlerInterface;
    }
    namespace IO {
        class ConnectionIO;
    }
}

namespace K {
namespace IO {
namespace Deprecated {

//! Buffered connection.
class BufferedConnection : public virtual Core::Deprecated::ConnectionStreamInterface {
  public:
    //! Creates a new buffered connection for the specified UNIX file descriptor.
    /*!
     *  \param fd
     *  The connection takes ownership over the UNIX file descriptor. Bad file descriptors (<c>nullopt</c> or negative
     *  ones) are accepted, with the connection being initialized to error state right away.
     *
     *  \param resultAcceptor
     *  The result acceptor is optional.
     */
    BufferedConnection(
        std::optional<int> fd, int bufferSizeThreshold, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    BufferedConnection(const BufferedConnection &other)             = delete;
    BufferedConnection &operator=(const BufferedConnection &other)  = delete;
    BufferedConnection(const BufferedConnection &&other)            = delete;
    BufferedConnection &operator=(const BufferedConnection &&other) = delete;
    ~BufferedConnection();

    //! <b>[Thread safe]</b> Triggers error state.
    /*!
     *  This method is thread safe. (Note that the class as a whole is not.)
     *
     *  Error state will hit in a deferred fashion - the object is not guaranteed to already be in error state when the
     *  method returns.
     */
    void TriggerErrorState();
    bool Register(const std::shared_ptr<Core::RawStreamHandlerInterface> &handler) override;
    void Unregister(const std::shared_ptr<Core::RawStreamHandlerInterface> &handler) override;
    int WriteBlocking(const void *data, int dataSize) override;
    bool ErrorState() const override;
    std::optional<Error> StreamError() const override;
    void SetCloseResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) override;

  private:
    class SharedState;

    std::shared_ptr<SharedState> sharedState_;

    std::shared_ptr<K::IO::ConnectionIO>  connectionIO_;
    std::optional<int>                    fd_;
    std::shared_ptr<Core::ResultAcceptor> closeResultAcceptor_;
};

}    // Deprecated.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DEPRECATED_BUFFEREDCONNECTION_H_
