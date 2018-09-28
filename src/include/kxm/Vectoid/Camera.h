#ifndef KXM_VECTOID_CAMERA_H_
#define KXM_VECTOID_CAMERA_H_

#include <kxm/Vectoid/CoordSysCore.h>

namespace kxm {
namespace Vectoid {

//! This scene graph node represents a camera.
/*! 
 *  \ingroup Vectoid
 */ 
class Camera : public CoordSysCore {
  public:
    Camera(const Camera &other) = delete;
    Camera &operator=(const Camera &other) = delete;
    
  protected:
    Camera() {}
};

}    // Namespace Vectoid.
}    // Namespace kxm.

#endif    // KXM_VECTOID_CAMERA_H_
