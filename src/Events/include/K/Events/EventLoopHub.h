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
    EventLoopHub();
    EventLoopHub(const EventLoopHub &other)            = delete;
    EventLoopHub &operator=(const EventLoopHub &other) = delete;
    EventLoopHub(EventLoopHub &&other)                 = delete;
    EventLoopHub &operator=(EventLoopHub &&other)      = delete;
    
    //! Allocates the resources for another client \ref EventLoop and returns a unique client id for it.
    int RegisterEventLoop();
    //! Unregisters the specified client loop (if such is registered).
    void UnregisterEventLoop(int clientLoopId);
    //! Registers the specified event id-to-slot mapping.
    /*!
     *  \return
     *  <c>false</c> in case of failure. In this case, the operation had no effect.
     */
    bool RegisterEventIdToSlotMapping(size_t id, int slot);
    //! Allows a client \ref EventLoop (thread) to post the events represented by the serialized event data in the
    //! specified buffer.
    void Post(int clientLoopId, const Core::Buffer &buffer, bool onlyPostToOthers);
    //! Allows a client \ref EventLoop (thread) to retrieve all events currently scheduled for it.
    /*!
     *  \param nonBlocking
     *  If set to <c>false</c> and there are currently no more events scheduled for the calling client \ref EventLoop,
     *  the method blocks the client loop's thread until new events arrive for it (or shutdown is requested).
     *
     *  \param buffer
     *  Some buffer no longer needed by the client \ref EventLoop. It will be swapped with another buffer containing all
     *  event data the hub currently has for the calling client \ref EventLoop.
     *
     *  \return <c>false</c> in case shutdown has been requested. The returned buffer will then be cleared.
     */
    bool GetEvents(int clientLoopId, std::unique_ptr<Core::Buffer> *buffer, bool nonBlocking);
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
            waiting           = false;
            shutDownRequested = false;
            inUse             = false;
        }
    };
    
    void DoPost(int clientLoopId, const Core::Buffer &buffer, bool onlyPostToOthers);
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

