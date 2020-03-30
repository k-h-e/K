#ifndef K_EVENTS_ACTORMAP_
#define K_EVENTS_ACTORMAP_

#include <cassert>
#include <vector>
#include <K/Events/ActorName.h>

namespace K {
namespace Events {

//! Maps actor names to user-definable data.
/*!
 *  \ingroup Events
 *
 *  The <c>ActorData</c> type must be copyable and default-constructible.
 */
template<class ActorData>
class ActorMap {
  public:
    ActorMap() {};
    ActorMap(const ActorMap &other)            = delete;
    ActorMap &operator=(const ActorMap &other) = delete;
    ActorMap(ActorMap &&other)                 = delete;
    ActorMap &operator=(ActorMap &&other)      = delete;
    //! Registers the specified data for the given actor name.
    /*!
     *  No data must already be registered for the actor name's id component.
     *
     *  The data gets copied.
     */
    void Register(const ActorName &name, const ActorData &data);
    //! Unregisters the specified actor and its associated data, if such an actor is present.
    /*!
     *  The actor name's incarnation number must match for an unregistration to occur!
     *
     *  If unregistration occurs, the respective data slot's content is replaced with a default-constructed data
     *  instance.
     */
    void Unregister(const ActorName &name);
    //! Returns the data associated with the specified actor name, or <c>nullptr</c> if such an actor is not present
    //! (including the case where \ref ActorName::IsNone() ).
    /*!
     *  The data remains owned by the <c>ActorMap</c>. The pointer may get invalidated by further operations on the
     *  <c>ActorMap</c>.
     */
    ActorData *Get(const ActorName &name);
    
  private:
    struct ActorInfo {
        uint32_t  incarnation;
        bool      dataValid;
        ActorData data;
        ActorInfo() : incarnation(0u), dataValid(false) {}
    };
    std::vector<ActorInfo> actors_;
};

template<class ActorData>
void ActorMap<ActorData>::Register(const ActorName &name, const ActorData &data) {
    assert(!name.IsNone());
    while ((int)actors_.size() <= name.Id()) {
        actors_.push_back(ActorInfo());
    }
    ActorInfo &info  = actors_[name.Id()];
    assert(!info.dataValid);
    info.incarnation = name.Incarnation();
    info.dataValid   = true;
    info.data        = data;
}

template<class ActorData>
void ActorMap<ActorData>::Unregister(const ActorName &name) {
    ActorData *data = Get(name);
    if (data) {
        ActorInfo &info = actors_[name.Id()];
        info.data       = ActorData();
        info.dataValid  = false;
    }
}

template<class ActorData>
ActorData *ActorMap<ActorData>::Get(const ActorName &name) {
    if (!name.IsNone() && (name.Id() < (int)actors_.size())) {
        ActorInfo &info = actors_[name.Id()];
        if (info.dataValid && (info.incarnation == name.Incarnation())) {
            return &info.data;
        }
    }
    return nullptr;
}

}    // Namespace Events.
}    // Namespace K.

#endif    // K_EVENTS_ACTORMAP_
