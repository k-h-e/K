#ifndef K_EVENTS_EVENTLOOPHUB_H_
#define K_EVENTS_EVENTLOOPHUB_H_

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
class EventLoopHub : public virtual EventReceiverInterface {
  public:
    class HandlerInterface : public virtual Core::Interface {
      public:
        virtual void OnEventsAvailable() = 0;
    };

    EventLoopHub();
    EventLoopHub(const EventLoopHub &other)            = delete;
    EventLoopHub &operator=(const EventLoopHub &other) = delete;
    EventLoopHub(EventLoopHub &&other)                 = delete;
    EventLoopHub &operator=(EventLoopHub &&other)      = delete;
    ~EventLoopHub()                                    = default;

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
     *  The handler's methods get activated by the thread posting the respective events to the hub. They must not call
     *  back into the hub.
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
    //! Allows a client \ref EventLoop (thread) to post the events represented by the specified serialized event data.
    void Post(int clientLoopId, const void *data, int dataSize, bool onlyPostToOthers);
    //! Allows a client \ref EventLoop (thread) to retrieve all events currently scheduled for it.
    /*!
     *  If there are currently no more events scheduled for the calling client \ref EventLoop, the method returns
     *  immediately.
     *
     *  \param buffer
     *  Some buffer no longer needed by the client \ref EventLoop. It will be swapped with another buffer containing all
     *  event data the hub currently has for the calling client \ref EventLoop.
     *
     *  \return <c>false</c> in case shutdown has been requested. The returned buffer will then be cleared.
     */
    bool GetEvents(int clientLoopId, std::unique_ptr<Core::Buffer> *buffer);
    //! Allows a client \ref EventLoop (thread) to retrieve all events currently scheduled for it.
    /*!
     *  If there are currently no more events scheduled for the calling client \ref EventLoop, the method blocks the
     *  client loop's thread until new events arrive for it, or the specified timeout has elapsed (if given), or
     *  shutdown is requested.
     *
     *  \param buffer
     *  Some buffer no longer needed by the client \ref EventLoop. It will be swapped with another buffer containing all
     *  event data the hub currently has for the calling client \ref EventLoop.
     *
     *  \return <c>false</c> in case shutdown has been requested. The returned buffer will then be cleared.
     */
    bool GetEvents(int clientLoopId, std::unique_ptr<Core::Buffer> *buffer,
                   std::optional<std::chrono::milliseconds> timeout);
    //! Asks all participating client \ref EventLoop s to finish running (but does not wait until that has happened).
    /*!
     *  Client \ref EventLoop s can check for whether shutdown is requested for them by inspecting the return value of
     *  \ref GetEvents().
     */
    void RequestShutDown();
    //! Asks the specified client \ref EventLoop to finish running (but does not wait until that has happened).
    /*!
     *  Client \ref EventLoop s can check for whether shutdown is requested for them by inspecting the return value of
     *  \ref GetEvents().
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
    
    void DoPost(int clientLoopId, const void *data, int dataSize, bool onlyPostToOthers);
    LoopInfo *GetLoopInfo(int clientLoopId);

    std::mutex                      lock_;
    std::vector<LoopInfo>           loops_;
    std::stack<int>                 unusedLoopSlots_;
    std::unordered_map<size_t, int> eventIdToSlotMap_;
    Core::Buffer                    eventsToSchedule_;
    bool                            shutDownRequested_;
};

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_EVENTLOOPHUB_H_

