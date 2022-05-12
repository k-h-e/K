////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/IO/Framework/InteractionConnectionEndPoint.h>

using std::shared_ptr;
using K::Core::Framework::NonBlockingIOStreamInterface;

namespace K {
namespace IO {
namespace Framework {

InteractionConnectionEndPoint::InteractionConnectionEndPoint(const shared_ptr<NonBlockingIOStreamInterface> &connection)
        : connection_{connection} {
    // Nop.
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
