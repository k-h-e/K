#ifndef K_IO_IO_H_
#define K_IO_IO_H_

#include <memory>
#include <K/Core/Interface.h>

namespace K {
namespace Core {
    class ThreadPool;
}
}

namespace K {
//! I/O.
namespace IO {

//! Central I/O mechanism.
/*!
 *  The class is thread-safe, i.e. all its public methods.
 */
class IO : public virtual K::Core::Interface {
  public:
    class ClientInterface : public virtual K::Core::Interface {
      public:
        //! Returns whether or not the client can accept more read data.
        virtual bool OnDataRead(void *data, int dataSize) = 0;
        virtual void OnReadyWrite() = 0;
        virtual void OnEof() = 0;
        virtual void OnError() = 0;
    };

    IO(const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    ~IO();
    IO(const IO &other)            = delete;
    IO &operator=(const IO &other) = delete;
    IO(IO &&other)                 = delete;
    IO &operator=(IO &&other)      = delete;

    //! Registers the specified file descriptor.
    /*!
     *  The client will get called on an arbitrary thread.
     *
     *  \return
     *  <c>false</c> in case the file descriptor could not be registered.
     */
    bool Register(int fd, ClientInterface *client);
    //! Unregisters the respective file descriptor, if it was registered.
    /*!
     *  When the method returns it is guaranteed that the file descriptor is no longer used, and that the respective
     *  cient is not called again.
     */
    void Unregister(int fd);
    //! Informs the I/O mechanism that the client for the specified file descriptor can read.
    void SetClientCanRead(int fd);
    //! Informs the I/O mechanism that the client for the specified file descriptor can write.
    void SetClientCanWrite(int fd);

  private:
    class SharedState;
    class Worker;

    std::shared_ptr<SharedState> sharedState_;
    std::shared_ptr<Worker>      worker_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_IO_H_
