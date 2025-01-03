/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_CONNECTIONIO_H_
#define K_IO_CONNECTIONIO_H_

#include <memory>

#include <K/Core/Interface.h>
#include <K/Core/UniqueHandle.h>

namespace K {
    namespace Core {
        class IoBufferInterface;
        class IoBuffers;
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
        //! Delivers another portion of read data to the client.
        /*!
         *  \return <c>false</c> in case the client asks to pause reading. The client must however always accept more
         *          read data passed via OnDataRead(). 
         */
        virtual bool OnDataRead(Core::UniqueHandle<Core::IoBufferInterface> buffer) = 0;
        //! Asks the client to provide data to write.
        /*!
         *  \return Data to write, or null-handle in case the client does currently not have any more data to write.
         */
        virtual Core::UniqueHandle<Core::IoBufferInterface> OnReadyWrite() = 0;
        virtual void OnIncompleteWrite(Core::UniqueHandle<Core::IoBufferInterface> buffer) = 0;
        virtual void OnCustomCall() = 0;
        virtual void OnEof() = 0;
        virtual void OnError() = 0;
    };

    ConnectionIO(const std::shared_ptr<Core::IoBuffers> &ioBuffers,
                 const std::shared_ptr<Core::ThreadPool> &threadPool);
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
     *  <c>false</c> in case the file descriptor could not be registered. This includes cases with negative file
     *  descriptors.
     */
    bool Register(const std::shared_ptr<ClientInterface> &client, int fd);
    //! Unregisters the respective client, if it was registered.
    /*!
     *  When the method returns it is guaranteed that the cient will not be called again, and that the associated file
     *  descriptor will no longer be used.
     */
    void Unregister(const std::shared_ptr<ClientInterface> &client, bool *outFinalStreamError);
    //! Informs the I/O mechanism that the specified client can read.
    void SetClientCanRead(ClientInterface *client);
    //! Informs the I/O mechanism that the specified client can write.
    void SetClientCanWrite(ClientInterface *client);
    //! Requests a custom call for the specified client.
    void RequestCustomCall(ClientInterface *client);
    //! Requests error state to be raised for the specified client.
    void RequestErrorState(ClientInterface *client);

  private:
    class SharedState;
    class Worker;
    struct WorkInfo;
    struct RegistrationInfo;
    struct UnregistrationInfo;

    std::shared_ptr<SharedState> sharedState_;
    std::shared_ptr<Worker>      worker_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_CONNECTIONIO_H_
