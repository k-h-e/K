//
//  FrameTimeEvent.h
//  kxm
//
//  Created by Kai Hergenröther on 10/29/14.
//
//


#ifndef KXM_ZARCH_FRAMETIMEEVENT_H_
#define KXM_ZARCH_FRAMETIMEEVENT_H_


#include <kxm/Zarch/Events/ZarchEvent.h>


namespace kxm {
namespace Zarch {

class FrameTimeEvent : public ZarchEvent {
  public:
    FrameTimeEvent();
    FrameTimeEvent(float timeS);
    const EventType &Type() { return type; }
    void Serialize(Core::Buffer *targetBuffer) const;
    void Deserialize(Core::Buffer::Reader *bufferReader);
    
    static const EventType type;
    
    float timeS_;
};

}    // Namespace Zarch.
}    // Namespace kxm.

#endif    // KXM_ZARCH_FRAMETIMEEVENT_H_

