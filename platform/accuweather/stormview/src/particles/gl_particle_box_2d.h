#ifndef _GL_PARTICLE_BOX_2D_H_
#define _GL_PARTICLE_BOX_2D_H_

#include "glm/fwd.hpp"
class ParticleBuffer;

class GLParticleBox2D
{
public:
   virtual ~GLParticleBox2D();

   static bool Init();
   static void DeleteParticleBox2D();
   static void DrawGL(
         const glm::mat4x4& mat,
         const ParticleBuffer& buffer
         );

   static void Enable(const glm::mat4x4& mat);
   static void Disable();
protected:

private:
   GLParticleBox2D();
   static GLParticleBox2D& GetInstance();
   GLParticleBox2D(const GLParticleBox2D& ) = delete;
   GLParticleBox2D& operator=(GLParticleBox2D) = delete;

   bool m_init = false;

   unsigned int m_program;

   unsigned int m_u_mat;
   unsigned int m_u_color;
   unsigned int m_u_res;

   unsigned int m_a_vert;
   unsigned int m_a_pos;
   unsigned int m_a_angle;
   unsigned int m_a_length;
   unsigned int m_a_width;

   unsigned int m_vert_buffer;
   unsigned int m_elem_buffer;

};


#endif // end of include guard: _GL_PARTICLE_BOX_2D_H_ 
