////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_FRAMEWORK_RUNLOOP_H_
#define K_CORE_FRAMEWORK_RUNLOOP_H_

#include <condition_variable>
#include <deque>
#include <mutex>
#include <unordered_set>
#include <vector>
#include <K/Core/Interface.h>

namespace K {
namespace Core {
//! Framework- (<c>RunLoop</c>-) enabled variants of core classes.
namespace Framework {

//! Run loop.
/*!
 *  Implements a reactive model for organizing a given thread's control flow. Handlers for associated input sources are
 *  guaranteed to all get invoked on this thread, in the following referred to as run loop thread. These handlers may
 *  call back into the run loop.
 *
 *  The run loop object and all its associated input sources together with their handlers are all intended to live in
 *  the associated thread.
 */
class RunLoop : public virtual Core::Interface {
  public:
    class ClientInterface : public virtual Core::Interface {
      public:
        //! Used by the run loop to inject activity into the client on the run loop thread.
        virtual void Activate() = 0;
    };

    RunLoop();
    RunLoop(const RunLoop &other)            = delete;
    RunLoop &operator=(const RunLoop &other) = delete;
    RunLoop(RunLoop &&other)                 = delete;
    RunLoop &operator=(RunLoop &&other)      = delete;
    ~RunLoop()                               = default;

    //! Called by the associated thread to, well, run the run loop.
    void Run();
    //! Asks the run loop to terminate.
    /*!
     *  If the loop is currently running (i.e. executing <c>Run()</c>), it will terminate (return from <c>Run()</c>) at
     *  the next possible time.
     */
    void RequestTermination();
    //! Adds the specified client, returning the assigned client id.
    /*!
     *  The client will exclusively get activated (called) on the run loop thread. The client may call back into the run
     *  loop object.
     */
    int AddClient(ClientInterface *client);
    //! Removes the specified client.
    /*!
     *  When the method returns, it is guaranteed that the client will not be called again.
     */
    void RemoveClient(int client);
    //! <b>[Thread safe]</b> Requests the specified client to be activated on the run loop thread.
    /*!
     *  This method is thread safe.
     */
    void RequestActivation(int client);

  private:
    struct ClientInfo {
        ClientInterface *client;

        ClientInfo();
        ClientInfo(const ClientInfo &other)            = delete;
        ClientInfo &operator=(const ClientInfo &other) = delete;
        ClientInfo(ClientInfo &&other)                 = default;
        ClientInfo &operator=(ClientInfo &&other)      = default;
        ~ClientInfo()                                  = default;

        void Reset();
    };

    // Expects lock to be held.
    void ClearPendingActivation(int client, std::unique_lock<std::mutex> &critical);

    std::mutex              lock_;                     // Protects everything below...
    std::condition_variable stateChanged_;
    std::vector<ClientInfo> clients_;
    std::deque<int>         freeClientSlots_;
    std::unordered_set<int> clientsToActivate_;
    std::deque<int>         clientActivationOrder_;
    bool                    terminationRequested_;
};

}    // Namespace Framework.
}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_FRAMEWORK_RUNLOOP_H_
