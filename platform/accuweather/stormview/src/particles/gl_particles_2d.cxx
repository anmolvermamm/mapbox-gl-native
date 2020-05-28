#include "gl_particles_2d.h"


#include "glm/mat4x4.hpp"
#include "../glow/glow.h"
#include "../glow/glow_shader_mem.h"
#include "gl_debug.h"
#include "../glow/gl_include.h"
#include "../projections.h"
#include "../logging.h"
#include "../config.h"

#include "../color.h"
#include "particle_buffer.h"

#include <iostream>

extern bool DEBUG_GL_LOAD;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GLParticles2D::DeleteParticles2D()
{
   auto& me = GLParticles2D::GetInstance();
   glDeleteProgram(me.m_program);
   me.m_init = false;
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GLParticles2D::GLParticles2D()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GLParticles2D::~GLParticles2D()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GLParticles2D& GLParticles2D::GetInstance()
{
   static GLParticles2D the_instance;
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GLParticles2D::InitGLParticles2D()
{
   auto& me = GetInstance();
   //We have been initialized already
   if( me.m_init ) return me.m_init;
 
   GL_DEBUG_MACRO();

   bool success  = Glow::compileProgramFromShaderMem(
         me.m_program,
         GlowShaderMem::m_particle_2d_vert, 
         GlowShaderMem::m_particle_2d_frag  );

   if( !success )
   {
      printf("Failed to gl_particles_2d_load\n");
      printf("Error %s\n", Glow::GetError(me.m_program).c_str());
      me.m_init = false;
   }

   GL_DEBUG_MACRO();

   me.m_mat_uloc = Glow::GetUniformLocation( me.m_program , "u_matrix" );
   me.m_color_uloc = Glow::GetUniformLocation( me.m_program , "u_color" );
   me.m_vert_aloc = Glow::GetAttributeLocation( me.m_program , "a_vertex" );
   me.m_size_uloc = Glow::GetUniformLocation( me.m_program , "u_point_size" );
   GL_DEBUG_MACRO();

   if( DEBUG_GL_LOAD )
   {
      printf("ParticlesDot\n");
      printf("program: %d, mat_uloc: %d, color_uloc: %d\n",
            me.m_program, me.m_mat_uloc, me.m_color_uloc);
      printf("size_uloc: %d, ver_aloc: %d\n",
            me.m_size_uloc, me.m_vert_aloc);
   }

   me.m_init = success;

   return me.m_init;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GLParticles2D::DrawGL( 
const glm::mat4x4& mat ,
const ParticleBuffer& buffer )
{
   static int count = -1;
   ++count;
   auto& me = GLParticles2D::GetInstance();

   if( !me.m_init ) return;

   glUseProgram( me.m_program );


#ifdef DESKTOP
   glEnable(GL_POINT_SPRITE);
   glPointSize( 2.0f );
#endif

   glEnable( GL_BLEND);
   glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA ); 

   glEnableVertexAttribArray(me.m_vert_aloc);
   
   if( !count ) GL_DEBUG_MACRO();

   glBindBuffer( GL_ARRAY_BUFFER , buffer.m_buffer );
   glVertexAttribPointer( me.m_vert_aloc , 2 , GL_FLOAT , GL_FALSE , 2*sizeof(GLfloat) , (void*)0 );
   if( !count ) GL_DEBUG_MACRO();

   glUniformMatrix4fv( me.m_mat_uloc , 1 , GL_FALSE , &mat[0][0] );
   if( !count ) GL_DEBUG_MACRO();

   Color color( 1.0f , 1.0 , 1.0 , buffer.m_alpha );
   glUniform4f( me.m_color_uloc , 
         color.red() , color.green() , color.blue() , color.alpha() );

   glUniform1f( me.m_size_uloc , 2.0f );
   glDrawArrays( GL_POINTS , 0 , buffer.m_num_points );

   if( !count ) GL_DEBUG_MACRO();

   glDisableVertexAttribArray(me.m_vert_aloc);

   glDisable( GL_BLEND);

#ifdef DESKTOP
   glDisable(GL_POINT_SPRITE);
#endif

   glUseProgram( 0 );

   if( !count ) GL_DEBUG_MACRO();
}
