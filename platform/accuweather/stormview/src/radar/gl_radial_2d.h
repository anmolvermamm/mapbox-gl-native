#ifndef _GL_RADIAL_2D_H_
#define _GL_RADIAL_2D_H_


#include <glm/fwd.hpp>
#include "pointf.h"


class RadialBuffer;

class GLRadial2D
{
public:
   static bool GLInitRadial2D();
   static void DeleteRadial2D();

   static void DrawGL(
         const glm::mat4x4& mat,
         const RadialBuffer& buffer,
         float radius,
         PointF& center,
         float alpha
   );


private:
   GLRadial2D();
   static GLRadial2D& Get();
   GLRadial2D(const GLRadial2D& ) = delete;
   GLRadial2D& operator=(GLRadial2D) = delete;


   bool m_init = false;
   unsigned int m_program;

   unsigned int m_u_mat;
   unsigned int m_u_loaded;
   unsigned int m_u_data;
   unsigned int m_u_alpha;
   unsigned int m_u_center;
   unsigned int m_u_radius;
   unsigned int m_u_is_smir;
   unsigned int m_u_is_8bit;
   unsigned int m_u_palette;
   unsigned int m_u_palette_mixed;
   unsigned int m_u_palette_ice;
   unsigned int m_u_palette_snow;
   unsigned int m_u_palette_lv2_velocity;
   unsigned int m_u_translate;

   unsigned int m_a_texcoord;//aloc
   unsigned int m_a_vert;//aloc

   unsigned int m_elem_buffer;//elm buffer

   unsigned int m_palette_nws_precip_tex;
   unsigned int m_palette_accu_precip_tex;
   unsigned int m_palette_smir_tex;
   unsigned int m_palette_mixed_tex;
   unsigned int m_palette_ice_tex;
   unsigned int m_palette_snow_tex;
};


#endif // end of include guard: _GL_RADIAL_2D_H_ 
