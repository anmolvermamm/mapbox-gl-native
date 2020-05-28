#include "glow_point_sprite.h"


#include "glow.h"
#include "glow_shader_mem.h"
#include "gl_debug.h"
#include "gl_include.h"


#include "glm/mat4x4.hpp"
#include "pointf.h"
#include "color.h"
#include "../config.h"

#include <iostream>
#include <string>

extern bool DEBUG_GL_LOAD;

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void GlowPointSprite::DeleteGlowPointSprite()
{
   auto& me = Get();
   Glow::DeleteBuffer(me.m_vertex_buffer);
   glDeleteProgram(me.m_program);
   me.m_init = false;
   return;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GlowPointSprite::InitGlowPointSprite()
{
   auto& me = Get();
   //We have been initialized already
   if( me.m_init ) return me.m_init;
 
   GL_DEBUG_MACRO();

   bool success  = Glow::compileProgramFromShaderMem(
         me.m_program,
         GlowShaderMem::m_point_2d_vert, 
         GlowShaderMem::m_point_2d_frag  );

   if( !success )
   {
      printf("Failed to glow_point_sprite_load\n Error %s\n",
            Glow::GetError(me.m_program).c_str());
      me.m_init = false;
   }

   GL_DEBUG_MACRO();

   
   me.m_vertex_buffer = Glow::CreateBuffer();
   GL_DEBUG_MACRO();

   me.m_vert_aloc = Glow::GetAttributeLocation( me.m_program , "a_position" );

   me.m_mat_uloc = Glow::GetUniformLocation( me.m_program , "u_matrix" );
   me.m_point_size_uloc = Glow::GetUniformLocation( me.m_program , "u_point_size" );
   me.m_line_color_uloc = Glow::GetUniformLocation( me.m_program , "u_line_color");
   me.m_fill_color_uloc = Glow::GetUniformLocation( me.m_program , "u_fill_color");
   me.m_border_width_uloc = Glow::GetUniformLocation(me.m_program, "u_border_width");
   me.m_translate_uloc = Glow::GetUniformLocation(me.m_program, "u_translate");
   //me.m_z_order_uloc = Glow::GetUniformLocation(me.m_program, "u_z_order");

   GL_DEBUG_MACRO();

   if( DEBUG_GL_LOAD )
   {
      printf("PointSprite\n");
      printf("program: %d, vert_buffer: %d, vert_aloc: %d\n",
            me.m_program, me.m_vertex_buffer, me.m_vert_aloc);
      printf("mat_uloc: %d, fcolor_uloc: %d, lcolor_uloc: %d\n",
            me.m_vert_aloc, me.m_fill_color_uloc, me.m_line_color_uloc);
      printf("point_size_uloc: %d\n", me.m_point_size_uloc);
   }

   me.m_init = success;

   return me.m_init;


}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GlowPointSprite::Draw(
const glm::mat4x4& mat , 
const PointF& point 
)
{
   static int count = -1;
   ++count;
   count = count % 60;

   auto& me = GlowPointSprite::Get();

   if( !me.m_init ) return false;

   glUseProgram( me.m_program );

   glEnable( GL_BLEND);
   float point_size = Config::GetPointSpriteSize();
#ifdef DESKTOP
   glEnable(GL_POINT_SPRITE);
   glPointSize(point_size);
#endif

   glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA ); 

   glEnableVertexAttribArray(me.m_vert_aloc);
   
   if( !count ) GL_DEBUG_MACRO();
   
   Glow::LoadBuffer( me.m_vertex_buffer , point );
   if( !count ) GL_DEBUG_MACRO();
   glVertexAttribPointer( me.m_vert_aloc , 2 , GL_FLOAT , GL_FALSE , 2*sizeof(GLfloat) , (void*)0 );

   if( !count ) GL_DEBUG_MACRO();

   glUniformMatrix4fv( me.m_mat_uloc , 1 , GL_FALSE , &mat[0][0] );
   if( !count ) GL_DEBUG_MACRO();

   float alpha = 0.8;
   Color line_color( 0.1f , 0.0 , 0.0 , alpha );
   glUniform4f( me.m_line_color_uloc , 
         line_color.red() , line_color.green() , line_color.blue() , line_color.alpha() );

   Color fill_color( 0.8f , 0.0 , 0.0 , alpha );
   glUniform4f( me.m_fill_color_uloc , 
         fill_color.red() , fill_color.green() , fill_color.blue() , fill_color.alpha() );

   glUniform1f(me.m_translate_uloc, 0.0f);
   glUniform1f( me.m_point_size_uloc , point_size );
   glUniform1f( me.m_z_order_uloc, 0.2);
   if( !count ) GL_DEBUG_MACRO();

   glDrawArrays( GL_POINTS , 0 , 1 );

   if( !count ) GL_DEBUG_MACRO();

   glDisableVertexAttribArray(me.m_vert_aloc);

   glDisable(GL_BLEND);
#ifdef DESKTOP
   glDisable(GL_POINT_SPRITE);
#endif

   glUseProgram( 0 );

   if( !count ) GL_DEBUG_MACRO();

   GL_DEBUG_CLEAR();

   return true;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GlowPointSprite::Draw(
const glm::mat4x4& mat ,
const PointF& point,
const Color color,
const float z_order
)
{
   static int count = -1;
   ++count;
   count = count % 60;

   auto& me = GlowPointSprite::Get();

   if( !me.m_init ) return false;

   glUseProgram( me.m_program );

   glEnable( GL_BLEND);
   float point_size = Config::GetPointSpriteSize();
#ifdef DESKTOP
   glEnable(GL_POINT_SPRITE);
   glPointSize(point_size);
#endif

   glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

   glEnableVertexAttribArray(me.m_vert_aloc);

   if( !count ) GL_DEBUG_MACRO();

   //LoadVertexData( me.m_vertex_buffer , verts , 0.3 ); //Z??
   Glow::LoadBuffer( me.m_vertex_buffer , point );
   if( !count ) GL_DEBUG_MACRO();
   glVertexAttribPointer( me.m_vert_aloc , 2 , GL_FLOAT , GL_FALSE , 2*sizeof(GLfloat) , (void*)0 );

   if( !count ) GL_DEBUG_MACRO();

   glUniformMatrix4fv( me.m_mat_uloc , 1 , GL_FALSE , &mat[0][0] );
   if( !count ) GL_DEBUG_MACRO();

   float alpha = 0.8;
   Color line_color( 0.0f , 0.0f , 0.0f , alpha );
   glUniform4f( me.m_line_color_uloc ,
         line_color.red() , line_color.green() , line_color.blue() , line_color.alpha() );

   glUniform4f( me.m_fill_color_uloc ,
         color.red() , color.green() , color.blue() , color.alpha() );

   glUniform1f(me.m_translate_uloc, 0.0f);
   glUniform1f( me.m_z_order_uloc, z_order);
   glUniform1f( me.m_point_size_uloc , point_size );
   if( !count ) GL_DEBUG_MACRO();

   glDrawArrays( GL_POINTS , 0 , 1 );

   if( !count ) GL_DEBUG_MACRO();

   glDisableVertexAttribArray(me.m_vert_aloc);

   glDisable(GL_BLEND);
#ifdef DESKTOP
   glDisable(GL_POINT_SPRITE);
#endif

   glUseProgram( 0 );

   if( !count ) GL_DEBUG_MACRO();

   GL_DEBUG_CLEAR();

   return true;

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool GlowPointSprite::Draw(const glm::mat4x4& mat, unsigned buffer, unsigned count,
      PrimitiveStyle style, std::unique_ptr<AnimationManager> &animation_manager)
{
   static int log_count = -1;
   ++log_count;
   log_count = log_count % 60;

   auto& me = GlowPointSprite::Get();

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

   if( !count ) GL_DEBUG_MACRO();

   glUniformMatrix4fv( me.m_mat_uloc , 1 , GL_FALSE , &mat[0][0] );
   if( !count ) GL_DEBUG_MACRO();

   if(animation_manager != nullptr)
   {
      glUniform4f(me.m_line_color_uloc, style.border_color.red(),
            style.border_color.green(), style.border_color.blue(),
            animation_manager->GetCurrentOpacity());
   }
   else
   {
      glUniform4f(me.m_line_color_uloc,
            style.border_color.red(), style.border_color.green(), style.border_color.blue(),
            style.border_color.alpha());
   }

   if(animation_manager != nullptr)
   {
      glUniform4f(me.m_fill_color_uloc, style.color.red(), style.color.green(),
            style.color.blue(), animation_manager->GetCurrentOpacity());
   }
   else
   {
      glUniform4f(me.m_fill_color_uloc,
            style.color.red(), style.color.green(), style.color.blue(), style.color.alpha());
   }

   glUniform1f(me.m_translate_uloc, 0.0);
   glUniform1f(me.m_point_size_uloc, 2.0 * Config::GetScreenDensity() * style.radius);
   glUniform1f(me.m_border_width_uloc, style.border_width);

   if( !log_count ) GL_DEBUG_MACRO();

   glDrawArrays( GL_POINTS , 0 , count );

   if(Config::IsOverIDL())
   {
      PointF geo_point(GeoPointToMercatorPoint(PointF(360.0, 0.0)));
      glUniform1f(me.m_translate_uloc, geo_point.x());
      glDrawArrays( GL_POINTS , 0 , count );
   }

   if( !log_count ) GL_DEBUG_MACRO();

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
GlowPointSprite& GlowPointSprite::Get()
{
   static GlowPointSprite the_instance;
   return the_instance;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
GlowPointSprite::GlowPointSprite()
{

}
