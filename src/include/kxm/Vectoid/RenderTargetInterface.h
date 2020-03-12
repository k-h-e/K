#ifndef KXM_VECTOID_RENDERTARGETINTERFACE_H_
#define KXM_VECTOID_RENDERTARGETINTERFACE_H_

#include <memory>
#include <kxm/Core/Interface.h>

namespace kxm {
namespace Vectoid {

class AgeColoredParticles;
class Camera;
class CoordSys;
class GeometryInterface;
class Geode;
class Glyphs;
class Particles;
class ParticlesRenderer;
class PerspectiveProjection;
class SceneGraphNode;
class TestTriangle;
class TextConsole;

//! Interface to <c>Vectoid</c> render targets.
/*!
 *  \ingroup Vectoid
 *
 *  A <c>Vectoid</c> render target acts as rendering-platform-specific factory for scene graph nodes and associated
 *  resources.
 */
class RenderTargetInterface : public virtual Core::Interface {
  public:
    virtual void SetSceneGraph(const std::shared_ptr<SceneGraphNode> &sceneGraphRoot) = 0;
    //! Renders a frame using the current scene graph state.
    virtual void RenderFrame() = 0;
  
    virtual std::shared_ptr<AgeColoredParticles> NewAgeColoredParticles(const std::shared_ptr<Particles> &particles)
        = 0;
    virtual std::shared_ptr<Camera> NewCamera() = 0;
    virtual std::shared_ptr<CoordSys> NewCoordSys() = 0;
    virtual std::shared_ptr<Geode> NewGeode(const std::shared_ptr<GeometryInterface> &geometry) = 0;
    virtual std::shared_ptr<Glyphs> NewGlyphs() = 0;
    virtual std::shared_ptr<ParticlesRenderer> NewParticlesRenderer(const std::shared_ptr<Particles> &particles) = 0;
    virtual std::shared_ptr<PerspectiveProjection> NewPerspectiveProjection() = 0;
    virtual std::shared_ptr<TestTriangle> NewTestTriangle() = 0;
    virtual std::shared_ptr<TextConsole> NewTextConsole(int width, int height, float glyphWidth, float glyphHeight,
                                                        const std::shared_ptr<Glyphs> &glyphs) = 0;
};

}    // Namespace Vectoid.
}    // Namespace kxm.

#endif    // KXM_VECTOID_RENDERTARGETINTERFACE_H_
