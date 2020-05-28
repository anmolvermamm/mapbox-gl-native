#include "basic_icon.h"


#include "glow.h"
#include "glow_shader_mem.h"
#include "gl_debug.h"
#include "gl_include.h"
#include "../projections.h"

#include "glm/mat4x4.hpp"
#include "../pointf.h"
#include "../color.h"
#include "../config.h"

#include <iostream>
#include <string>

extern bool DEBUG_GL_LOAD;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void BasicIcon::DeleteBasicIcon()
{
   auto& me = Get();
   Glow::DeleteBuffer(me.m_vertex_buffer);
   glDeleteProgram(me.m_program);
   me.m_init = false;
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool BasicIcon::InitBasicIcon()
{
   auto& me = Get();
   //We have been initialized already
   if( me.m_init ) return me.m_init;

   GL_DEBUG_MACRO();

   bool success = Glow::compileProgramFromShaderMem(
         me.m_program,
         GlowShaderMem::m_point_2d_vert_tex,
         GlowShaderMem::m_point_2d_frag_tex);

   if( !success )
   {
      printf("Failed to basic_icon_load\n Error %s\n",
            Glow::GetError(me.m_program).c_str());
      me.m_init = false;
   }

   GL_DEBUG_MACRO();


   me.m_vertex_buffer = Glow::CreateBuffer();
   GL_DEBUG_MACRO();

   me.m_vert_aloc = Glow::GetAttributeLocation( me.m_program , "a_position" );

   me.m_mat_uloc = Glow::GetUniformLocation( me.m_program , "u_matrix" );
   me.m_point_size_uloc = Glow::GetUniformLocation( me.m_program , "u_point_size" );
   //me.m_fill_color_uloc = Glow::GetUniformLocation( me.m_program , "u_fill_color");
   me.m_texture_uloc = Glow::GetUniformLocation(me.m_program, "u_tex");
   me.m_translate_uloc = Glow::GetUniformLocation(me.m_program, "u_translate");


   GL_DEBUG_MACRO();

   if( DEBUG_GL_LOAD )
   {
      printf("IconSprite\n");
      printf("program: %d, vert_buffer: %d, vert_aloc: %d\n",
            me.m_program, me.m_vertex_buffer, me.m_vert_aloc);
      printf("mat_uloc: %d, fcolor_uloc: %d\n",
            me.m_vert_aloc, me.m_fill_color_uloc);
      printf("point_size_uloc: %d\n", me.m_point_size_uloc);
   }

   me.m_init = success;

   return me.m_init;


}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool BasicIcon::Draw(const glm::mat4x4& mat, unsigned buffer, unsigned count,
      unsigned int texture, PrimitiveStyle style)
{
   static int log_count = -1;
   ++log_count;
   log_count = log_count % 60;

   auto& me = BasicIcon::Get();

   if( !me.m_init ) return false;

   glUseProgram( me.m_program );

   glEnable( GL_BLEND);
#ifdef DESKTOP
   glEnable(GL_POINT_SPRITE);
   glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

   glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

   glEnableVertexAttribArray(me.m_vert_aloc);

   if( !log_count ) GL_DEBUG_MACRO();

   glBindBuffer(GL_ARRAY_BUFFER , buffer );
   glVertexAttribPointer(me.m_vert_aloc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
         (void*)0 );

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture);
   glUniform1i(me.m_texture_uloc, 0);

   glUniformMatrix4fv( me.m_mat_uloc , 1 , GL_FALSE , &mat[0][0] );
   glUniform1f(me.m_translate_uloc, 0.0);
   glUniform1f(me.m_point_size_uloc, Config::GetScreenDensity() * style.radius);

   glDrawArrays( GL_POINTS , 0 , count );

   if(Config::IsOverIDL())
   {
      PointF geo_point(GeoPointToMercatorPoint(PointF(360.0, 0.0)));
      glUniform1f(me.m_translate_uloc, geo_point.x());
      glDrawArrays( GL_POINTS , 0 , count );
   }

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glDisableVertexAttribArray(me.m_vert_aloc);

   glDisable(GL_BLEND);
#ifdef DESKTOP
   glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
   glDisable(GL_POINT_SPRITE);
#endif

   glUseProgram( 0 );

   if( !log_count ) GL_DEBUG_MACRO();

   GL_DEBUG_CLEAR();

   return true;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
BasicIcon& BasicIcon::Get()
{
   static BasicIcon the_instance;
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
BasicIcon::BasicIcon()
{

}
