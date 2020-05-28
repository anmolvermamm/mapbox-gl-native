#ifndef _GL_SHADER_MEM_H_
#define _GL_SHADER_MEM_H_

#include <string>

class GlowShaderMem
{
public:

   static std::string m_image_2d_vert;
   static std::string m_image_2d_frag;

   static std::string m_particle_2d_vert;
   static std::string m_particle_2d_frag;

   static std::string m_point_2d_vert;
   static std::string m_point_2d_frag;

   static std::string m_basic_sprite_vert;
   static std::string m_basic_sprite_frag;

   static std::string m_line_2d_vert;
   static std::string m_line_2d_frag;
   
   static std::string m_pbox_2d_vert;
   static std::string m_pbox_2d_frag;

   static std::string m_default_vert;
   static std::string m_default_frag;

   static std::string m_radial_vert;
   static std::string m_radial_frag;

   static std::string m_point_2d_vert_tex;
   static std::string m_point_2d_frag_tex;

};


#endif // end of include guard: _GL_SHADER_MEM_H_ 
