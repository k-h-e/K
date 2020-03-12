#ifndef KXM_ZARCH_VIDEO_OPENGL_SIMPLEGEOMETRYRENDERER_H_
#define KXM_ZARCH_VIDEO_OPENGL_SIMPLEGEOMETRYRENDERER_H_

#include <kxm/Zarch/Video/SimpleGeometryRenderer.h>

#include <vector>
#include <kxm/Vectoid/OpenGL/OpenGL.h>

namespace kxm {
namespace Zarch {
namespace Video {
namespace OpenGL {

class RenderTarget;

//! Renders simple geometry.
/*!
 *  \ingroup ZarchVideoOpenGL
 */
class SimpleGeometryRenderer : public Video::SimpleGeometryRenderer {
  public:
    friend class RenderTarget;
    
    SimpleGeometryRenderer(const SimpleGeometryRenderer &other) = delete;
    SimpleGeometryRenderer &operator=(const SimpleGeometryRenderer &other) = delete;
    void Render();
    
  private:
    SimpleGeometryRenderer(const std::shared_ptr<SimpleGeometry> &geometry);
  
    int                  numTriangles_;
    std::vector<GLfloat> vertexArray_,
                         colorArray_;
};

}    // Namespace OpenGL.
}    // Namespace Video.
}    // Namespace Zarch.
}    // Namespace kxm.

#endif    // KXM_ZARCH_VIDEO_OPENGL_SIMPLEGEOMETRYRENDERER_H_