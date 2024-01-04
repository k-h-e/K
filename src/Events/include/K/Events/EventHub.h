/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_EVENTS_EVENTHUB_H_
#define K_EVENTS_EVENTHUB_H_

#include <memory>
#include <queue>
#include <thread>
#include <unordered_map>
#include <stack>
#include <condition_variable>
#include <K/Core/Buffer.h>
#include <K/Events/EventReceiverInterface.h>

namespace K {
namespace Events {

//! Hub joining together multiple \ref EventLoop s where each lives in an individual thread.
/*!
 *  The class is threadsafe (i.e. all public methods).
 */
class EventHub : public virtual EventReceiverInterface {
  public:
    class HandlerInterface : public virtual Core::Interface {
      public:
        virtual void OnEventsAvailable() = 0;
    };

    EventHub();
    EventHub(const EventHub &other)            = delete;
    EventHub &operator=(const EventHub &other) = delete;
    EventHub(EventHub &&other)                 = delete;
    EventHub &operator=(EventHub &&other)      = delete;
    ~EventHub()                                = default;

    //! Allocates the resources for another client \ref EventLoop and returns a unique client id for it.
    int RegisterEventLoop();
    //! Unregisters the specified client loop (if such is registered).
    /*!
     *  If a handler was registered for the client loop, it is guaranteed that it will not be called again after the
     *  method has returned.
     */
    void UnregisterEventLoop(int clientLoopId);
    //! Registers the specified handler for the specified client loop.
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered handler. An unregistered handler will not be called
     *  again after the method has returned.
     *
     *  The handler is expected to outlive the hub, or as long until it is unregistered via <c>RegisterHandler()</c>, or
     *  until the respective client loop is unregistered via <c>UnregisterEventLoop()</c>.
     *
     *  The handler's methods get activated by the thread posting events to the hub or requesting shutdown. They must
     *  not call back into the hub.
     *
     *  After registering a handler, a client should initially fetch its events to kick-start the events-available
     *  signalling.
     */
    void RegisterHandler(int clientLoopId, HandlerInterface *handler);
    //! Registers the specified event id-to-slot mapping.
    /*!
     *  \return
     *  <c>false</c> in case of failure. In this case, the operation had no effect.
     */
    bool RegisterEventIdToSlotMapping(size_t id, int slot);
    //! Allows a client \ref EventLoop (thread) to both retrieve all events currently scheduled for it and submit all
    //! posted events it currently has.
    /*!
     *  If there are currently no more events scheduled for the calling client \ref EventLoop, the method returns
     *  immediately.
     *
     *  \param buffer
     *  Buffer holding events to be submitted to the hub. Will be swapped with another buffer containing all event data
     *  the hub currently has for the calling client \ref EventLoop.
     *
     *  \return <c>false</c> in case shutdown has been requested. The returned buffer will then be cleared.
     */
    bool Sync(int clientLoopId, std::unique_ptr<Core::Buffer> *buffer);
    //! Allows a client \ref EventLoop (thread) to both retrieve all events currently scheduled for it and submit all
    //! posted events it currently has.
    /*!
     *  If there are currently no more events scheduled for the calling client \ref EventLoop, the method blocks the
     *  client loop's thread until new events arrive for it, or the specified timeout has elapsed (if given), or
     *  shutdown is requested.
     *
     *  \param buffer
     *  Buffer holding events to be submitted to the hub. Will be swapped with another buffer containing all event data
     *  the hub currently has for the calling client \ref EventLoop.
     *
     *  \return <c>false</c> in case shutdown has been requested. The returned buffer will then be cleared.
     */
    bool Sync(int clientLoopId, std::unique_ptr<Core::Buffer> *buffer,
              std::optional<std::chrono::milliseconds> timeout);
    //! Allows a client \ref EventLoop (thread) to submit the events represented by the specified serialized event data
    //! to the hub.
    void Submit(int clientLoopId, const void *data, int dataSize, bool onlyDeliverToOthers);
    //! Asks all participating client \ref EventLoop s to finish running (but does not wait until that has happened).
    /*!
     *  Client \ref EventLoop s can check for whether shutdown is requested for them by inspecting the return value of
     *  \ref Sync().
     */
    void RequestShutDown();
    //! Asks the specified client \ref EventLoop to finish running (but does not wait until that has happened).
    /*!
     *  Client \ref EventLoop s can check for whether shutdown is requested for them by inspecting the return value of
     *  \ref Sync().
     */
    void RequestShutDown(int clientLoopId);

    virtual void Post(const Event &event);
    
  private:
    struct LoopInfo {
        std::unique_ptr<Core::Buffer>            buffer;
        std::unique_ptr<std::condition_variable> stateChanged;
        HandlerInterface                         *handler;
        bool                                     waiting;
        bool                                     shutDownRequested;
        bool                                     inUse;

        LoopInfo()
                : buffer(new Core::Buffer()),
                  stateChanged(new std::condition_variable()) {
            Reset();
        }

        void Reset() {
            buffer->Clear();
            handler           = nullptr;
            waiting           = false;
            shutDownRequested = false;
            inUse             = false;
        }
    };
    
    void DoSubmit(std::unique_lock<std::mutex> &critical, int clientLoopId, const void *data, int dataSize,
                  bool onlyDeliverToOthers);
    LoopInfo *GetLoopInfo(std::unique_lock<std::mutex> &critical, int clientLoopId);

    std::mutex                      lock_;
    std::vector<LoopInfo>           loops_;
    std::stack<int>                 unusedLoopSlots_;
    std::unordered_map<size_t, int> eventIdToSlotMap_;
    Core::Buffer                    eventsToSchedule_;
    bool                            shutDownRequested_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENTHUB_H_

