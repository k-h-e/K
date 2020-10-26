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
    class ReadHandlerInterface : public virtual K::Core::Interface {
        virtual void OnReadyRead() = 0;
    };
    class WriteHandlerInterface : public virtual K::Core::Interface {
        virtual void OnReadyWrite() = 0;
    };

    IO(const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    ~IO();
    IO(const IO &other)            = delete;
    IO &operator=(const IO &other) = delete;
    IO(IO &&other)                 = delete;
    IO &operator=(IO &&other)      = delete;

    //! Registers the specified file descriptor.
    /*!
     *  By passing the respective handlers, you can register the file descriptor for reading, for writing, or for both.
     *  Pass <c>nullptr</c> for a given handler if you don't need the respective transfer direction.
     *
     *  The handlers will get called on an arbitrary thread.
     *
     *  \return
     *  <c>false</c> in case the file descriptor could not be registered.
     */
    bool Register(int fd, ReadHandlerInterface *reader, WriteHandlerInterface *writer);
    //! Unregisters the respective file descriptor, if it was registered.
    /*!
     *  When the method returns it is guaranteed that the file descriptor is no longer used, and that the respective
     *  handlers are not called again.
     */
    void Unregister(int fd);

  private:
    class SharedState;
    class Worker;

    std::shared_ptr<SharedState> sharedState_;
    std::shared_ptr<Worker>      worker_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_IO_H_
