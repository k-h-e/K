#ifndef KXM_ZARCH_VIDEO_TERRAINRENDERER_H_
#define KXM_ZARCH_VIDEO_TERRAINRENDERER_H_

#include <vector>
#include <memory>
#include <kxm/Vectoid/OpenGL.h>
#include <kxm/Vectoid/GeometryInterface.h>

namespace Vectoid {
    class RenderContext;
}

namespace kxm {
namespace Zarch {

class Terrain;
class MapParameters;

namespace Video {

//! Renders the terrain.
/*!
 *  \ingroup ZarchVideo
 */
class TerrainRenderer : public virtual Vectoid::GeometryInterface {
  public:
    TerrainRenderer(std::shared_ptr<Terrain> terrain,
                    std::shared_ptr<MapParameters> mapParameters);
    
    //! Sets the observer's position in the <c>xz</c>-plane.
    void SetObserverPosition(float x, float z);
    //! Gets the observer's position in the <c>xz</c>-plane.
    void GetObserverPosition(float *outX, float *outZ);
    void Render(Vectoid::RenderContext *context);
    
  private:
    TerrainRenderer(const TerrainRenderer &other);
    TerrainRenderer &operator=(const TerrainRenderer &other);
    
    std::shared_ptr<Terrain>             terrain_;
    std::shared_ptr<const MapParameters> mapParameters_;
    float                                observerX_, observerZ_;
    std::vector<GLfloat>                 vertices_;
};

}    // Namespace Video.
}    // Namespace Zarch.
}    // Namespace kxm.

#endif    // KXM_ZARCH_VIDEO_TERRAINRENDERER_H_