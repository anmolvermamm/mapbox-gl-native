#ifndef _GL_PARTICLES_2D_H_
#define _GL_PARTICLES_2D_H_

#include "glm/fwd.hpp"
class ParticleBuffer;

class GLParticles2D
{
public:
   virtual ~GLParticles2D();
   static GLParticles2D& GetInstance();
   
   static bool InitGLParticles2D();
   static void DeleteParticles2D();
   static void DrawGL( 
         const glm::mat4x4& mat ,
         const ParticleBuffer& buffer );

private:
   GLParticles2D();
   GLParticles2D(const GLParticles2D& ) = delete;
   GLParticles2D& operator=(GLParticles2D&) = delete;

   unsigned int m_program = 0;
   unsigned int m_mat_uloc = 0;
   unsigned int m_color_uloc = 0;
   unsigned int m_size_uloc = 0;

   unsigned int m_vert_aloc = 0;

   bool m_init = false;
   
};

#endif // end of include guard: _GL_PARTICLES_2D_H_ 
