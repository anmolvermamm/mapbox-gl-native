#ifndef _BASIC_ICON_H_
#define _BASIC_ICON_H_

#include "glm/fwd.hpp"
#include "color.h"
#include "default_vbo.h"

class PointF;

class BasicIcon
{
public:


   static bool InitBasicIcon();
   static void DeleteBasicIcon();

   static bool Draw(
      const glm::mat4x4& mat ,
      unsigned buffer,
      unsigned count,
      unsigned texture,
      PrimitiveStyle style
   );


private:
   static BasicIcon& Get();
   BasicIcon();
   BasicIcon(const BasicIcon& ) = delete;
   BasicIcon& operator=(BasicIcon&) = delete;


   bool m_init = false;
   unsigned int m_program = 0;
   unsigned int m_vertex_buffer = 0;

   unsigned int m_mat_uloc;
   unsigned int m_point_size_uloc;
   unsigned int m_texture_uloc;
   unsigned int m_translate_uloc;

   unsigned int m_fill_color_uloc;

   unsigned int m_vert_aloc;

};


#endif // end of include guard: _BASIC_ICON_H_
