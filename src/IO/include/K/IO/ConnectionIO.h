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

//! Central I/O mechanism for connection streams.
/*!
 *  The class is thread-safe, i.e. all its public methods.
 */
class ConnectionIO : public virtual K::Core::Interface {
  public:
    class ClientInterface : public virtual K::Core::Interface {
      public:
        //! Provides the client with read data.
        /*!
         *  \return
         *  <c>true</c> in case the client can accept more read data.
         */
        virtual bool OnDataRead(const void *data, int dataSize) = 0;
        //! Asks the client to provide data to write.
        /*!
         *  \return
         *  The number of bytes to write that were put into the specified buffer. <c>0</c> means the client does not
         *  have any more data to write.
         */
        virtual int OnReadyWrite(void *buffer, int bufferSize) = 0;
        virtual void OnIncompleteWrite(const void *unwrittenData, int unwrittenDataSize) = 0;
        virtual void OnCustomCall() = 0;
        virtual void OnEof() = 0;
        virtual void OnError() = 0;
    };

    ConnectionIO(const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    ~ConnectionIO();
    ConnectionIO(const ConnectionIO &other)            = delete;
    ConnectionIO &operator=(const ConnectionIO &other) = delete;
    ConnectionIO(ConnectionIO &&other)                 = delete;
    ConnectionIO &operator=(ConnectionIO &&other)      = delete;

    //! Registers the specified client and its associated file descriptor.
    /*!
     *  The client methods will get called on an arbitrary thread.
     *
     *  \return
     *  <c>false</c> in case the file descriptor could not be registered.
     */
    bool Register(ClientInterface *client, int fd);
    //! Unregisters the respective client, if it was registered.
    /*!
     *  When the method returns it is guaranteed that the cient will not be called again, and that the associated file
     *  descriptor will no longer be used.
     */
    void Unregister(ClientInterface *client, bool *outError);
    //! Informs the I/O mechanism that the specified client can read.
    void SetClientCanRead(ClientInterface *client);
    //! Informs the I/O mechanism that the specified client can write.
    void SetClientCanWrite(ClientInterface *client);
    //! Requests a custom call for the specified client.
    void RequestCustomCall(ClientInterface *client);

  private:
    class SharedState;
    class Worker;

    std::shared_ptr<SharedState> sharedState_;
    std::shared_ptr<Worker>      worker_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_IO_H_
