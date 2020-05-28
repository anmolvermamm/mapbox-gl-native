#ifndef _GLOW_POINT_SPRITE_H_
#define _GLOW_POINT_SPRITE_H_

#include "glm/fwd.hpp"
#include "color.h"
#include "../animation_manager.h"
#include "default_vbo.h"

class PointF;

class GlowPointSprite
{
public:


   static bool InitGlowPointSprite();
   static void DeleteGlowPointSprite();

   static bool Draw(
      const glm::mat4x4& mat , 
      const PointF& point 
   );

   static bool Draw(
      const glm::mat4x4& mat , 
      unsigned buffer,
      unsigned count,
      PrimitiveStyle style, std::unique_ptr<AnimationManager> &animation_manager
   );

   static bool Draw(const glm::mat4x4& mat ,
   const PointF& point,
   const Color color
   , const float z_order);

private:
   static GlowPointSprite& Get();
   GlowPointSprite();
   GlowPointSprite(const GlowPointSprite& ) = delete;
   GlowPointSprite& operator=(GlowPointSprite&) = delete;
   

   bool m_init = false;
   unsigned int m_program = 0;
   unsigned int m_program_texture = 0;
   unsigned int m_vertex_buffer = 0;

   unsigned int m_mat_uloc;
   unsigned int m_point_size_uloc;
   unsigned int m_texture_uloc;

   unsigned int m_line_color_uloc;
   unsigned int m_fill_color_uloc;
   unsigned int m_z_order_uloc;
   unsigned int m_border_width_uloc;
   unsigned int m_translate_uloc;

   unsigned int m_vert_aloc;
   //unsigned int m_text_coord_aloc;

};


#endif // end of include guard: _GLOW_POINT_SPRITE_H_ 
