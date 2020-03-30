#include <K/Events/Actions.h>

#include <cstdio>

using namespace K::Core;

namespace K {
namespace Events {

int Actions::Register(ActionInterface *action) {
    int storageId;
    actions_.Get(0, &storageId) = action;
    return storageId;
}

void Actions::Unregister(int id) {
    actions_.Put(id);
}

void Actions::Execute() {
    int num = 0;
    for (ActionInterface *action : actions_.Iterate(0)) {
        action->ExecuteAction();
        ++num;
    }
}

}    // Namespace Events.
}    // Namespace K.
