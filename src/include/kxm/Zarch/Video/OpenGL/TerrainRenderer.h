#ifndef KXM_ZARCH_VIDEO_OPENGL_TERRAINRENDERER_H_
#define KXM_ZARCH_VIDEO_OPENGL_TERRAINRENDERER_H_

#include <kxm/Zarch/Video/TerrainRenderer.h>

#include <kxm/Vectoid/OpenGL/OpenGL.h>

namespace kxm {
namespace Zarch {
namespace Video {
namespace OpenGL {

class RenderTarget;

//! Renders the terrain.
/*!
 *  \ingroup ZarchVideoOpenGL
 */
class TerrainRenderer : public Video::TerrainRenderer {
  public:
    friend class RenderTarget;
  
    TerrainRenderer(const TerrainRenderer &other) = delete;
    TerrainRenderer &operator=(const TerrainRenderer &other) = delete;
    void Render();
    
  private:
    TerrainRenderer(const std::shared_ptr<Terrain> &terrain,
                    const std::shared_ptr<MapParameters> &mapParameters);
    
    std::vector<GLfloat> vertices_;
};

}    // Namespace OpenGL.
}    // Namespace Video.
}    // Namespace Zarch.
}    // Namespace kxm.

#endif    // KXM_ZARCH_VIDEO_OPENGL_TERRAINRENDERER_H_