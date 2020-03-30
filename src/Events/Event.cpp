#include <K/Events/Event.h>

#include <functional>

namespace K {
namespace Events {

Event::EventType::EventType(const char *aName)
        : name(aName),
          id(std::hash<const char *>()(aName)) {
}

void Event::Serialize(Core::Buffer *targetBuffer) const {
}

void Event::Deserialize(Core::Buffer::Reader *bufferReader) {
}

}    // Namespace Events.
}    // Namespace K.
