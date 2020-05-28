#include "gl_particle_box_2d.h"
#include "gl_debug.h"

#include "../glow/gl_include.h"

#include "../color.h"
#include "../config.h"
#include "particle_buffer.h"
#include "../glow/glow_shader_mem.h"
#include "../projections.h"
#include "particle_config.h"
#include <iostream>
#include <glm/mat4x4.hpp>
#include <vector>

extern bool DEBUG_GL_LOAD;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GLParticleBox2D::DeleteParticleBox2D()
{
   auto& me = GLParticleBox2D::GetInstance();
   Glow::DeleteBuffer(me.m_vert_buffer);
   Glow::DeleteBuffer(me.m_elem_buffer);
   glDeleteProgram(me.m_program);
   me.m_init = false;
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GLParticleBox2D::~GLParticleBox2D()
{
 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GLParticleBox2D::GLParticleBox2D()
{
 
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GLParticleBox2D& GLParticleBox2D::GetInstance()
{
   static GLParticleBox2D the_instance;
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GLParticleBox2D::Init()
{
   GL_DEBUG_MACRO();
   auto& me = GLParticleBox2D::GetInstance();

   bool success = Glow::compileProgramFromShaderMem( 
         me.m_program ,
         GlowShaderMem::m_pbox_2d_vert , 
         GlowShaderMem::m_pbox_2d_frag );

   if( !success )
   {
      printf("Failed to gl_particle_box_2d_load\n");
      printf("Error %s\n", Glow::GetError(me.m_program).c_str());
      me.m_init = false;
   }

   me.m_u_mat = Glow::GetUniformLocation( me.m_program , "u_matrix" );
   me.m_u_color = Glow::GetUniformLocation( me.m_program ,"u_color" );
   me.m_u_res = Glow::GetUniformLocation( me.m_program ,   "u_res" );

   me.m_a_vert = Glow::GetAttributeLocation(  me.m_program ,  "a_vertex" );
   me.m_a_pos = Glow::GetAttributeLocation(   me.m_program ,  "a_position" );
   me.m_a_angle = Glow::GetAttributeLocation( me.m_program ,   "a_angle" );
   me.m_a_length = Glow::GetAttributeLocation(me.m_program ,   "a_length" );
   me.m_a_width = Glow::GetAttributeLocation( me.m_program ,  "a_width" );

   std::vector<float> verts { 
      0.0 ,  1.0 ,
      0.0 , -1.0 ,
      1.0 , -1.0 ,
      1.0 ,  1.0 };
   
   std::vector<unsigned int> elements {
      0 , 1 , 2, 2,  3, 0
   };

   me.m_vert_buffer = Glow::CreateBuffer( verts );
   me.m_elem_buffer = Glow::CreateElemBuffer( elements );
   
   if( DEBUG_GL_LOAD ) 
   {
      printf("ParticleBox2D\n");
      printf("program: %d, Umat: %d, Ucolor: %d, URes: %d\n",
            me.m_program, me.m_u_mat, me.m_u_color, me.m_u_res);
      printf("Avert: %d, Apos: %d, Aangle: %d, Alength: %d\n",
            me.m_a_vert, me.m_a_pos, me.m_a_angle, me.m_a_length);
      printf("Awidth: %d\n", me.m_a_width);
   }
   DEBUG_GL_LOAD = false;

   return success;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GLParticleBox2D::Enable(const glm::mat4x4& mat)
{

   auto& me = GLParticleBox2D::GetInstance();
   glUseProgram( me.m_program );

   glEnableVertexAttribArray( me.m_a_vert );
   glEnableVertexAttribArray( me.m_a_pos );
   glEnableVertexAttribArray( me.m_a_angle );
   glEnableVertexAttribArray( me.m_a_length );
   glEnableVertexAttribArray( me.m_a_width );
   
   glBindBuffer( GL_ARRAY_BUFFER , me.m_vert_buffer );
   glVertexAttribPointer( me.m_a_vert, 2 , GL_FLOAT , GL_FALSE , 2*sizeof(GLfloat) , (void*)0 );
   glVertexAttribDivisor( me.m_a_vert, 0 );

   glUniformMatrix4fv( me.m_u_mat , 1 , GL_FALSE , &mat[0][0] );

   glVertexAttribDivisor( me.m_a_pos, 1 );
   glVertexAttribDivisor( me.m_a_angle, 1 );
   glVertexAttribDivisor( me.m_a_length, 1 );
   glVertexAttribDivisor( me.m_a_width, 1 );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER ,  me.m_elem_buffer );

   float res = ParticleConfig::GetResolution().x();
   glUniform1f( me.m_u_res , res );

   glEnable( GL_BLEND);
   glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA ); 

#ifdef DESKTOP
   glEnable(GL_POLYGON_SMOOTH);
#endif
   GL_DEBUG_MACRO();

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GLParticleBox2D::DrawGL(
      const glm::mat4x4& ,
      const ParticleBuffer& buffer
)
{
   //UNUSED(mat);
   if(!buffer.m_num_points)
   {
      printf("Particle buffer is empty!");
      return;
   }

   auto& me = GLParticleBox2D::GetInstance();

   glBindBuffer( GL_ARRAY_BUFFER , buffer.m_buffer );
   glVertexAttribPointer( me.m_a_pos , 2 , GL_FLOAT , GL_FALSE , 5*sizeof(GLfloat) , 0 );
   glVertexAttribPointer( me.m_a_angle , 1 , GL_FLOAT , GL_FALSE , 5*sizeof(GLfloat) , (void*)(sizeof(GLfloat)*2) );
   glVertexAttribPointer( me.m_a_length , 1 , GL_FLOAT , GL_FALSE , 5*sizeof(GLfloat) , (void*)(sizeof(GLfloat)*3) );
   glVertexAttribPointer( me.m_a_width , 1 , GL_FLOAT , GL_FALSE , 5*sizeof(GLfloat) , (void*)(sizeof(GLfloat)*4) );

   Color color( 1.0f, 1.0f , 1.0f , buffer.m_alpha );
   glUniform4f( me.m_u_color , 
         color.red() , color.green() , color.blue() , color.alpha() );
   glDrawElementsInstanced( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 ,buffer.m_num_points );

   GL_DEBUG_MACRO();
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GLParticleBox2D::Disable()
{
 
   auto& me = GLParticleBox2D::GetInstance();
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER , 0 );
   glBindBuffer( GL_ARRAY_BUFFER , 0 );
   GL_DEBUG_MACRO();

#ifdef DESKTOP
   glDisable(GL_POLYGON_SMOOTH);
#endif

   glVertexAttribDivisor( me.m_a_vert, 0 );
   glDisableVertexAttribArray(me.m_a_vert);

   glVertexAttribDivisor( me.m_a_pos , 0 );
   glDisableVertexAttribArray(me.m_a_pos);

   glVertexAttribDivisor( me.m_a_angle, 0 );
   glDisableVertexAttribArray(me.m_a_angle);

   glVertexAttribDivisor( me.m_a_length, 0 );
   glDisableVertexAttribArray(me.m_a_length);

   glVertexAttribDivisor( me.m_a_width, 0 );
   glDisableVertexAttribArray(me.m_a_width);

   glUseProgram( 0 );

   glDisable( GL_BLEND);
}
