#ifndef _PARTICLE_BUFFER_H_
#define _PARTICLE_BUFFER_H_

#include "glow.h"

class ParticleBuffer
{
public:
   ~ParticleBuffer()
   {
      if( m_owns && m_buffer )
      {
         Glow::DeleteBuffer( m_buffer );
         m_buffer = 0;
      }
   }

   float getTime() { return m_time; }

   unsigned int m_num_points = 0;
   unsigned int m_buffer = 0;
   float m_time = 0.0;
   float m_alpha = 1.0;
   bool m_owns = false;
   
};


#endif // end of include guard: _PARTICLE_BUFFER_H_ 
