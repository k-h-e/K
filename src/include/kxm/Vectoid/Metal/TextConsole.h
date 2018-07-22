#ifndef KXM_VECTOID_METAL_TEXTCONSOLE_H_
#define KXM_VECTOID_METAL_TEXTCONSOLE_H_

#include <kxm/Vectoid/TextConsole.h>

namespace kxm {
namespace Vectoid {
namespace Metal {

//! Text console geometry.
/*! 
 *  \ingroup VectoidMetal
 */
class TextConsole : public Vectoid::TextConsole {
  public:
    TextConsole(int width, int height, float glyphWidth, float glyphHeight, std::shared_ptr<Vectoid::Glyphs> glyphs);
    TextConsole(const TextConsole &other)            = delete;
    TextConsole &operator=(const TextConsole &other) = delete;
    TextConsole(TextConsole &&other)                 = delete;
    TextConsole &operator=(TextConsole &&other)      = delete;
    void Render();
};

}    // Namespace Metal.
}    // Namespace Vectoid.
}    // Namespace kxm.


#endif    // KXM_VECTOID_METAL_TEXTCONSOLE_H_
