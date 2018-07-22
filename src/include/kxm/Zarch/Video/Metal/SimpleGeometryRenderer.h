#ifndef KXM_ZARCH_VIDEO_METAL_SIMPLEGEOMETRYRENDERER_H_
#define KXM_ZARCH_VIDEO_METAL_SIMPLEGEOMETRYRENDERER_H_

#include <kxm/Zarch/Video/SimpleGeometryRenderer.h>

#include <vector>

namespace kxm {

namespace Vectoid {
namespace Metal {

class Context;

}
}

namespace Zarch {
namespace Video {
namespace Metal {

//! Renders simple geometry.
/*!
 *  \ingroup ZarchVideoMetal
 */
class SimpleGeometryRenderer : public Video::SimpleGeometryRenderer {
  public:
    SimpleGeometryRenderer(const std::shared_ptr<Vectoid::Metal::Context> &context, const std::shared_ptr<SimpleGeometry> &geometry);
    SimpleGeometryRenderer(const SimpleGeometryRenderer &other) = delete;
    SimpleGeometryRenderer &operator=(const SimpleGeometryRenderer &other) = delete;
    void Render();
    
  private:
    std::shared_ptr<Vectoid::Metal::Context> context_;
};

}    // Namespace Metal.
}    // Namespace Video.
}    // Namespace Zarch.
}    // Namespace kxm.

#endif    // KXM_ZARCH_VIDEO_METAL_SIMPLEGEOMETRYRENDERER_H_
