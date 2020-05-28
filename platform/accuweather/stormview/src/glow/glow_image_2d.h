#ifndef _GLOW_IMAGE_2D_H_
#define _GLOW_IMAGE_2D_H_

#include "glm/fwd.hpp"
class RectF;

class GlowImage2D
{
public:
   static bool InitGlowImage2D();
   static void DeleteGlowImage2D();
   //static void Setup();
   //static void Disable();

   //static void Draw(
   //      unsigned int m_texture_ref, 
   //      const RectF& verts );
   static void Draw(
      const glm::mat4x4& mat , 
      unsigned int m_texture_ref, 
      const RectF& verts );


private:
   static GlowImage2D& Get();
   GlowImage2D();
   GlowImage2D(const GlowImage2D& ) = delete;
   GlowImage2D& operator=(GlowImage2D&) = delete;

   bool m_init = false;
   unsigned int m_program = 0;
   unsigned int m_vertex_buffer = 0;
   unsigned int m_texture_buffer = 0;
   //unsigned int m_texture_stream_buffer;
   unsigned int m_element_buffer;
   unsigned int m_translate_uloc;

   unsigned int m_mat_uloc;
   unsigned int m_image_uloc;
   unsigned int m_has_data_uloc;

   unsigned int m_vert_aloc;
   unsigned int m_tex_aloc;


   
};



#endif // end of include guard: _GLOW_IMAGE_2D_H_ 
