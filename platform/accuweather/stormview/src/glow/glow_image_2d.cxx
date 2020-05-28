#include "glow_image_2d.h"

#include "glow.h"
#include "glow_shader_mem.h"
#include "gl_debug.h"
#include "gl_include.h"
#include "../pointf.h"
#include "../projections.h"
#include "../config.h"
#include "logging.h"

#include <iostream>
#include <string>
#include "rectf.h"
#include "glm/mat4x4.hpp"

extern bool DEBUG_GL_LOAD;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GlowImage2D::DeleteGlowImage2D()
{
   auto& me = Get();
   Glow::DeleteBuffer(me.m_vertex_buffer);
   Glow::DeleteBuffer(me.m_texture_buffer);
   Glow::DeleteBuffer(me.m_element_buffer);
   glDeleteProgram(me.m_program);
   me.m_init = false;
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GlowImage2D::GlowImage2D()
{
 
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GlowImage2D& GlowImage2D::Get()
{
   static GlowImage2D the_instance; 
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GlowImage2D::InitGlowImage2D()
{
   auto& me = Get();
   //We have been initialized already
   if( me.m_init ) return me.m_init;
 
   GL_DEBUG_MACRO();

   bool success  = Glow::compileProgramFromShaderMem(
         me.m_program,
         GlowShaderMem::m_image_2d_vert, 
         GlowShaderMem::m_image_2d_frag  );

   if( !success )
   {
      printf("Failed to glow_image_2d_load\n");
      printf("Error %s\n", Glow::GetError(me.m_program).c_str());
      me.m_init = false;
   }

   GL_DEBUG_MACRO();

   auto tex = RectF{ 0.0 , 1.0 , 0.0 , 1.0 };
   auto elem = std::vector<unsigned int>{0 , 1 , 2 , 2 , 3, 0 };
   
   me.m_vertex_buffer = Glow::CreateBuffer();
   me.m_texture_buffer = Glow::CreateBuffer( tex );
   me.m_element_buffer = Glow::CreateElemBuffer( elem );
   GL_DEBUG_MACRO();

   me.m_mat_uloc = Glow::GetUniformLocation( me.m_program , "matrix" );
   me.m_image_uloc = Glow::GetUniformLocation( me.m_program , "data" );
   me.m_has_data_uloc = Glow::GetUniformLocation( me.m_program , "has_data" );
   me.m_vert_aloc = Glow::GetAttributeLocation( me.m_program , "vertex_in" );
   me.m_tex_aloc = Glow::GetAttributeLocation( me.m_program , "texcoord_in" );
   me.m_translate_uloc = Glow::GetUniformLocation(me.m_program, "u_translate");
   GL_DEBUG_MACRO();

   if( DEBUG_GL_LOAD )
   {
      printf("Image 2D\n");
      printf("program: %d, vert_buff: %d, texture_buffer: %d, element_buffer: %d\n",
            me.m_program, me.m_vertex_buffer, me.m_texture_buffer, me.m_element_buffer);
      printf("mat_uloc: %d, image_uloc: %d, has_data_uloc: %d, vert_aloc: %d\n",
            me.m_mat_uloc, me.m_image_uloc, me.m_has_data_uloc, me.m_vert_aloc);
      printf("tex_aloc: %d\n", me.m_tex_aloc);
   }

   me.m_init = success;

   return me.m_init;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
//void GlowImage2D::Draw(
//unsigned int m_texture_ref, 
//const RectF& verts )
//{
// 
//}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GlowImage2D::Draw(
   const glm::mat4x4& mat , 
   unsigned int texture_ref, 
   const RectF& verts )
{
 
   static int count = -1;
   ++count;

   auto& me = GlowImage2D::Get();

   if( !me.m_init ) return;

   glUseProgram( me.m_program );

   glEnable( GL_BLEND);
   glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA ); 

   glEnableVertexAttribArray(me.m_vert_aloc);
   glEnableVertexAttribArray(me.m_tex_aloc);
   
   if( !count ) GL_DEBUG_MACRO();
   
   //LoadVertexData( me.m_vertex_buffer , verts , 0.3 ); //Z??
   Glow::LoadBuffer( me.m_vertex_buffer , verts , 0.3 );
   if( !count ) GL_DEBUG_MACRO();
   glVertexAttribPointer( me.m_vert_aloc , 3 , GL_FLOAT , GL_FALSE , 3*sizeof(GLfloat) , (void*)0 );

   if( !count ) GL_DEBUG_MACRO();

   //LoadTextureData( me.m_texture_stream_buffer , texture_coord);
   glBindBuffer( GL_ARRAY_BUFFER , me.m_texture_buffer );
   glVertexAttribPointer( me.m_tex_aloc , 2 , GL_FLOAT , GL_FALSE , 2*sizeof(GLfloat), (void*)0 );

   if( !count ) GL_DEBUG_MACRO();

   if( texture_ref != 0 )
   {
      glUniform1i(me.m_has_data_uloc , 1 );
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D , texture_ref);
      glUniform1i(me.m_image_uloc , 0 );
   } else {
      glUniform1i(me.m_has_data_uloc , 0 );
   }

   glUniformMatrix4fv( me.m_mat_uloc , 1 , GL_FALSE , &mat[0][0] );
   if( !count ) GL_DEBUG_MACRO();

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER ,  me.m_element_buffer );
   if( !count ) GL_DEBUG_MACRO();

   glUniform1f(me.m_translate_uloc, 0.0);
   glDrawElements( GL_TRIANGLES , 6 , GL_UNSIGNED_INT , 0 );

   if(Config::IsOverIDL())
   {
      PointF geo_point(GeoPointToMercatorPoint(PointF(360.0, 0.0)));
      glUniform1f(me.m_translate_uloc, geo_point.x());
      glDrawElements( GL_TRIANGLES , 6 , GL_UNSIGNED_INT , 0 );
   }

   if( !count ) GL_DEBUG_MACRO();

   glDisableVertexAttribArray(me.m_vert_aloc);
   glDisableVertexAttribArray(me.m_tex_aloc);

   glDisable( GL_BLEND);

   glUseProgram( 0 );

   //if_EVERY_N( 240 ) }
   if( !count ) GL_DEBUG_MACRO();

}









