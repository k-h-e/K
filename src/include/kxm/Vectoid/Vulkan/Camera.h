#ifndef KXM_VECTOID_VULKAN_CAMERA_H_
#define KXM_VECTOID_VULKAN_CAMERA_H_

#include <kxm/Vectoid/Camera.h>

namespace kxm {
namespace Vectoid {
namespace Vulkan {

class RenderTarget;
class Context;

//! This scene graph node represents a camera.
/*! 
 *  \ingroup VectoidVulkan
 */ 
class Camera : public Vectoid::Camera {
  public:
    friend class RenderTarget;
    
    Camera(const Camera &other) = delete;
    Camera &operator=(const Camera &other) = delete;
    void Render();
    
  private:
    Camera(const std::shared_ptr<Context> &context);
    
    std::shared_ptr<Context> context_;
};

}    // Namespace Vulkan.
}    // Namespace Vectoid.
}    // Namespace kxm.

#endif    // KXM_VECTOID_VULKAN_CAMERA_H_
