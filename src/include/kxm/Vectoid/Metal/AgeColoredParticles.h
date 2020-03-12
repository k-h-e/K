#ifndef KXM_VECTOID_METAL_AGECOLOREDPARTICLES_H_
#define KXM_VECTOID_METAL_AGECOLOREDPARTICLES_H_

#include <kxm/Vectoid/AgeColoredParticles.h>

namespace kxm {
namespace Vectoid {
namespace Metal {

class RenderTarget;
class Context;

//! Renders particles in different colors, depending of their age.
/*!
 *  \ingroup VectoidMetal
 */
class AgeColoredParticles : public Vectoid::AgeColoredParticles {
 public:
    friend class RenderTarget;
    
    AgeColoredParticles(const AgeColoredParticles &other) = delete;
    AgeColoredParticles &operator=(const AgeColoredParticles &other) = delete;
    void Render();
    
  private:
    AgeColoredParticles(const std::shared_ptr<Context> &context, const std::shared_ptr<Vectoid::Particles> &particles);
  
    std::shared_ptr<Context> context_;
};

}    // Namespace Metal.
}    // Namespace Vectoid.
}    // Namespace kxm.

#endif    // KXM_VECTOID_METAL_AGECOLOREDPARTICLES_H_