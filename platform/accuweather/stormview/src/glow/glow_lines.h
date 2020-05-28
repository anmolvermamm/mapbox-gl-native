#ifndef _GLOW_LINES_H_
#define _GLOW_LINES_H_

#include "glm/fwd.hpp"
#include <vector>
#include "../animation_manager.h"
#include "default_vbo.h"

//class LineF;
class PolygonF;

class GlowLines
{
public:
   static bool InitGlowLines();
   static void DeleteGlowLines();

   static bool Draw(
      const glm::mat4x4& mat , 
      const PolygonF& poly
   );

   static bool Draw(
      const glm::mat4x4& mat , 
      GeoType type,
      unsigned buffer ,
      unsigned element_buffer,
      unsigned count ,
      PrimitiveStyle style,
      bool closed, std::unique_ptr<AnimationManager> &animation_manager
   );


private:

   bool m_init = false;
   unsigned int m_program = 0;
   unsigned int m_vertex_buffer = 0;

   unsigned int m_mat_uloc;
   unsigned int m_line_color_uloc;
   unsigned int m_translate_uloc;

   unsigned int m_vert_aloc;

   static GlowLines& Get();
   GlowLines() = default;
   GlowLines(const GlowLines& ) = delete;
   GlowLines& operator=(GlowLines&) = delete;
   
};


#endif // end of include guard: _GLOW_LINES_H_ 
